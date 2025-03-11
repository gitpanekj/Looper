""" Implementation of the bound algorithm """

from looper.profiling import ProfilingManager
from expression import Expression
from looper.utils.constants import ZERO, ONE

type EdgeId = int
type LocalBounds = dict[EdgeId, Expression]


BoundAnalysisWatch = ProfilingManager.watch('bound_analysis')


@BoundAnalysisWatch(watch=True)
def local_bounds(dcp, norms) -> LocalBounds:
    local_bound_assignment : LocalBounds = {}
    
    
    unassigned_edges = set(dcp.get_edges())
    assigned_loop_edges = set()
    sccs = dcp.sccs()
    
    # Assign local bound to an edge where possible
    for edge_id in dcp.get_edges():

        # Edge is not a part of an SCC -> Local Bounds = 1
        src, dst = dcp.get_edge_nodes(edge_id)
        if (sccs[src] != sccs[dst]):
            local_bound_assignment[edge_id] = ONE
            unassigned_edges.remove(edge_id)
            continue
        
        # An edge is a part of an SCC, check whether a norm is decremented on the edge.
        label = dcp.get_edge_data(edge_id)
        constraints = label.constraints
        
        for norm in norms:
            constraint = constraints.get(str(norm), None)
            if not constraint:  continue
            
            # e <= e + c, c < 0
            
            if  constraint.x == constraint.y    and \
                str(constraint.y) == str(norm) and \
                constraint.c < ZERO:
                    local_bound_assignment[edge_id] = norm
                    unassigned_edges.remove(edge_id)
                    assigned_loop_edges.add(edge_id)
            
            # if edge is assigne LB, do not try other norms
            # NOTE this solutino is sufficient for now, but futher all the possible local boudns may be required
            # to store for a case that a local bounds does not lead to successfull computation
            if local_bound_assignment.get(edge_id, None):
                break
    
    if len(unassigned_edges) == 0:
        return local_bound_assignment
    
    # Local Bound Propagation
    edges_in_an_scc = {edge_id for edge_id in dcp.get_edges() if sccs[dcp.get_edge_nodes(edge_id)[0]] == sccs[dcp.get_edge_nodes(edge_id)[1]]}
    for edge_id in assigned_loop_edges:
        dcp.mark_as_erased(edge_id)
        _sccs = dcp.sccs()
        _edges_in_an_scc = {edge_id for edge_id in dcp.get_edges() if _sccs[dcp.get_edge_nodes(edge_id)[0]] == _sccs[dcp.get_edge_nodes(edge_id)[1]]}
        for _edge_id in edges_in_an_scc - _edges_in_an_scc:
            local_bound_assignment[_edge_id] = local_bound_assignment[edge_id]
        dcp.unmark_as_erased(edge_id)
        
        if len(unassigned_edges) == 0:
            return local_bound_assignment

    return local_bound_assignment # NOTE this means that an edge is not assigned a local bound


@BoundAnalysisWatch(watch=True)
def increment_sum(dcp, local_bound_assignment, norm) -> Expression:
    # for all edges where norm is incremented
    _increment_sum = Expression.create_constant(0)
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)    
        constraint = label.constraints.get(str(norm), None)
        if constraint and constraint.x == constraint.y and constraint.c > ZERO:
            _transition_bound = transition_bound(dcp, local_bound_assignment, edge_id)
            if not _transition_bound:
                return None
            
            _increment_sum += constraint.c * _transition_bound
    
    return _increment_sum

@BoundAnalysisWatch(watch=True)
def variable_bound(dcp, local_bound_assignment, norm) -> Expression:
    resets = []
    
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
        if constraint and constraint.x != constraint.y:
            _variable_bound = variable_bound(dcp, local_bound_assignment, constraint.y) + constraint.c
            if not _variable_bound:
                return None
            resets.append(_variable_bound)
    
    if not resets:
        return norm
    
    _increment_sum = increment_sum(dcp, local_bound_assignment, norm)
    if not _variable_bound:
        return None
    return resets[0] + _increment_sum # TODO change to max of resets
    

@BoundAnalysisWatch(watch=True)
def reset_sum(dcp, local_bound_assignment, norm) -> Expression:
    _reset_sum = Expression.create_constant(0)
    
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
        if constraint and constraint.x != constraint.y:
            _transition_bound = transition_bound(dcp, local_bound_assignment, edge_id)
            if not _transition_bound:
                return None
            _variable_bound = variable_bound(dcp, local_bound_assignment, constraint.y) + constraint.c # TODO change to max of resets
            if not _variable_bound:
                return None

            _reset_sum += _transition_bound * _variable_bound
    
    return _reset_sum

        

@BoundAnalysisWatch(watch=True)
def transition_bound(dcp, local_bound_assignment, edge_id) -> Expression:
    local_bound = local_bound_assignment[edge_id]
    if local_bound == ONE:
        return ONE.copy()
    
    _increment_sum = increment_sum(dcp, local_bound_assignment, local_bound)
    _reset_sum = reset_sum(dcp, local_bound_assignment, local_bound)
    
    if not _reset_sum or not _increment_sum:
        return None
    
    return _increment_sum + _reset_sum 

@BoundAnalysisWatch(watch=True)
def total_bound(dcp, norms) -> Expression | None:
    transition_bound_cache = {}
    variable_bound_cache = {}
    
    local_bound_assignment = local_bounds(dcp, norms)
    if len(local_bound_assignment) < len(dcp.get_edges()):
        return None
    
    back_edges = dcp.get_back_edges()
    total_bound = Expression.create_constant(0)
    for back_edge in back_edges:
        
        bound = transition_bound(dcp, local_bound_assignment, back_edge)
        
        if not bound:
            return None
        total_bound += bound
    
    return total_bound