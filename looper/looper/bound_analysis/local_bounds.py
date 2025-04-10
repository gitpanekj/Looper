from .analysis_profiler import BoundAnalysisWatch
from looper.utils import analysis_logger
from expression import Expression
from looper.utils import constants

# Types
type EdgeId = int
type LocalBounds = dict[EdgeId, Expression]


@BoundAnalysisWatch(watch=True)
def construct_local_bound_mapping(dcp, norms) -> LocalBounds:
    analysis_logger.info("Local Bound Assignemnt")
    
    local_bound_mapping : LocalBounds = {}
    
    edges = dcp.get_edges()
    unassigned_edges = set(dcp.get_edges())
    assigned_loop_edges = set()
    sccs = dcp.sccs()
    
    # Assign local bound to an edge where possible
    for edge_id in edges:
        # Edge is not a part of an SCC -> Local Bounds = 1
        src, dst = dcp.get_edge_nodes(edge_id)
        
        if (sccs[src] != sccs[dst]):
            local_bound_mapping[edge_id] = constants.ONE
            unassigned_edges.remove(edge_id)
            continue
        
        # An edge is a part of an SCC, check whether a norm is decremented on the edge.
        label = dcp.get_edge_data(edge_id)
        constraints = label.constraints
        
        for norm in norms:
            constraint = constraints.get(str(norm), None)
            if not constraint:
                continue
            
            # e <= e + c, c < 0
            if  constraint.x == constraint.y    and \
                str(constraint.y) == str(norm) and \
                constraint.c < constants.ZERO:
                    local_bound_mapping[edge_id] = norm
                    unassigned_edges.remove(edge_id)
                    assigned_loop_edges.add(edge_id)
            
            # if edge is assigne LB, do not try other norms
            # NOTE this solutino is sufficient for now, but futher all the possible local boudns may be required
            # to store for a case that a local bounds does not lead to successfull computation
            if local_bound_mapping.get(edge_id, None):
                break
            

    # Local Bound Propagation
    if len(unassigned_edges) != 0: 
        
        edges_in_an_scc = {edge_id for edge_id in dcp.get_edges() if sccs[dcp.get_edge_nodes(edge_id)[0]] == sccs[dcp.get_edge_nodes(edge_id)[1]]}
        for edge_id in assigned_loop_edges:
            dcp.mark_as_erased(edge_id)
            _sccs = dcp.sccs()
            _edges_in_an_scc = {edge_id for edge_id in dcp.get_edges() if _sccs[dcp.get_edge_nodes(edge_id)[0]] == _sccs[dcp.get_edge_nodes(edge_id)[1]]}
            for _edge_id in edges_in_an_scc - _edges_in_an_scc:
                local_bound_mapping[_edge_id] = local_bound_mapping[edge_id]
            dcp.unmark_as_erased(edge_id)
            
            if len(unassigned_edges) == 0:
                break

    # Loggin assignment results
    analysis_logger.log(f"\t[{'OK' if len(local_bound_mapping)==len(edges) else 'FAILED':^6}]")
    for edge  in dcp.get_edges():
        lb = local_bound_mapping.get(edge, None)
        analysis_logger.log(f"\t[{'OK' if lb else 'FAILED':^6}] LB({edge}) = {lb}")
        

    return local_bound_mapping