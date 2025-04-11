""" Difference Constraint Program (DCP) construction """

from z3 import Int, And, Solver, unsat
from itertools import chain, accumulate
from looper.profiling import ProfilingManager
from looper.utils import constants
from expression import Expression
from graphs import (DifferenceConstraintProgram as DCP,
                    DifferenceConstraint as DC,
                    DCPTransitionLabel,
                    LabeledTransitionSystem as LTS,
                    LTSTransitionCondition,
                    lts_to_dcp)
from looper.utils import analysis_logger

DCPConstructionWatch = ProfilingManager.watch('dcp_construction')



# TODO: helpers - should be encapsulated in DCP, LTS wrappers for Python
def get_transition_label(lts, edge):
    try:
        statements = lts.get_edge_data(edge).statements
    except ValueError:
        statements = []
    try:
        condition = lts.get_edge_data(edge).condition
    except ValueError:
        condition = LTSTransitionCondition()

    return (edge, statements, condition)

type Norm = Expression

@DCPConstructionWatch(watch=True)
def initial_set_of_norms(lts: LTS) -> set[Norm]:
    """ Return an initial set of norms.
        The initial set of norms consists of expressions derived from
        1) True branches of loop heads.
        2) NOTE other conditions may be added.
    """
    norms = []
    for edge in lts.get_edges():
        _, _, condition = get_transition_label(lts, edge)
        if len(condition.condition) == 0:   continue
        elif condition.true_branch: # and lts.is_loop_head(src): # only true branch of the loop head
            # infer the norm from the condition
            # x - y for x > y
            # x - y + 1 for x >= y
            n = condition.get_norm()
            if len(n[0]):
                norms.append(n)
    # NOTE: unpack norms for now, disabling compound norms
    norms = [norm[0][0] for norm in norms]
    
    return norms
            
def execute_transition(norm: Norm, statements) -> Norm:
    """ Symbolicaly executes statements over a norm expression
        Returns resulting norm expression.
    """
    
    norm_copy = norm.copy()
    
    if len(statements) == 0:
        return norm_copy
    
    for stmt in statements:    
        norm_copy.substitute(stmt.lhs, stmt.rhs.copy())
    
    norm_copy.expand()
    return norm_copy


@DCPConstructionWatch(watch=True)
def is_guard(condition, norm) -> bool:
    solver = Solver()
    ctx = {v.replace('.', '_'):Int(v) for v in norm.get_variable_names()}
    
    norms = condition.get_norm()
    if len(norms) >= 1 and len(norms[0]) >= 1: # TODO: patch solution, check whether a norm can be infferred from the condition
        ctx.update({v.replace('.', '_'):Int(v) for v in norms[0][0].get_variable_names()})
        
        z3_norm = eval(str(norm).replace('.', '_'), {}, ctx)
        z3_condition = eval(str(condition).replace('.', '_'), {}, ctx)
        formula = And(z3_condition, z3_norm <= 0)
        solver.add(formula)
        
        return solver.check() == unsat # c and not e > 0 is UNSAT then c -> e > 0 is valid
    return False


@DCPConstructionWatch(watch=True)
def infer_dcp_labels(lts: LTS, dcp: DCP) -> DCP:
    """ Label DCP by symbolically executing statements on the lts edge
        transitions and infer transition guards.
    """
    stable_norm_set: list[Norm] = []
    norms_to_process: list[Norm] = initial_set_of_norms(lts)
    edges = [get_transition_label(lts, edge) for edge in lts.get_edges()]
    
    # DCP edge labels are store in the mapping and assigned at once after 
    # DC construciton and trasnformatino over N
    dcp_edge_data = {edge: DCPTransitionLabel() for edge in lts.get_edges()}
    
    
    analysis_logger.log(f"\tInitial set of norms: {norms_to_process}")
    
    # For all norms
    while len(norms_to_process) != 0:
        norm = norms_to_process.pop()
        stable_norm_set.append(norm)
        analysis_logger.log(f"\tDerving transition labels for: {norm}")

        # For all edges
        for (edge, statements, condition) in edges:
            analysis_logger.log(f"\t  EDGE  {edge}")
            
            # 0) True/False branch Info
            dcp_edge_data[edge].true_branch = condition.true_branch
            
            # 1) check whether it is a guard
            if is_guard(condition, norm):
                analysis_logger.log(f"\t    - IS GUARD")
                dcp_edge_data[edge].add_guard(norm.copy())
                
            
            # 2) Infer difference constraint and check whether new norm is generated
            resulting_norm = execute_transition(norm, statements)
                
            # Find a norm such that is only differs in the constant term from the resulting norm
            for n in chain(stable_norm_set, norms_to_process):
                
                # Two norms differ only in constant part -> e_1 - e_2 = CONST
                diff = resulting_norm - n
                if diff.is_constant():
                    # Derive DC
                    dc = DC(norm, n, diff)    
                    dcp_edge_data[edge].add_dc(dc)
                    
                    analysis_logger.log(f"\t    - derived DC: {dc}")
                    break
            else: # No norm such that e_1 - e_2 = CONST was found, generate new norm e_2 with remove constant part
                
                # remove constant part
                const = resulting_norm.separate_constant()
                c = Expression.create_constant(const)
                resulting_norm -= c
                
                # Derived DC
                dc = DC(norm.copy(), resulting_norm.copy(), c.copy())
                dcp_edge_data[edge].add_dc(dc)
                analysis_logger.log(f"\t    - derived DC: {dc}")
                
                # do not generate norm built solely over constants
                if resulting_norm != constants.ZERO and \
                   not set(resulting_norm.get_variable_names()).issubset(set((name for name,_ in dcp.get_parameters()))):
                    norms_to_process.append(resulting_norm)
                    analysis_logger.log(f"\t    - adding norm {resulting_norm}")
    
    analysis_logger.log(f"\tFinal set of norms: {stable_norm_set}")
    
    
    
    # Guard propagation
    # nodes which have at least one incoming edge guarded, i.e. the guard is propagated through the node
    nodes_to_process = set(dcp.get_edge_nodes(edge)[1] for edge, label in dcp_edge_data.items() if len(label.guards) > 0)
    gaurd_propagation(dcp, dcp_edge_data, nodes_to_process)
    

    # transformation into DCP over N
    dcp_to_guareded_dcp(dcp_edge_data)
    
    
    # Setting DCP labels
    for edge, label in dcp_edge_data.items():
        dcp.set_edge_data(edge, label)
        
    return dcp, stable_norm_set


def gaurd_propagation(dcp, dcp_edge_data, nodes_to_process, loop_head_limit=None):    
    while len(nodes_to_process) > 0:
        node = nodes_to_process.pop()
        
        if dcp.is_loop_head(node):
            outgoing_edges = [(edge_id,dst) for edge_id, dst in dcp.get_outgoing_edges(node)]
            true_branch, false_branch = (outgoing_edges[0], outgoing_edges[1]) if dcp_edge_data[outgoing_edges[0][0]].true_branch else (outgoing_edges[1], outgoing_edges[0])
            
            ingoing_edges = [edge_id for edge_id, _ in dcp.get_ingoing_edges(node) if not dcp.is_back_edge(edge_id)]
            guards_expressions = {g_str: g for edge_id in ingoing_edges for g_str,g in dcp_edge_data[edge_id].guards.items()}
            
            shared_guards = set.intersection(*[set(dcp_edge_data[edge_id].guards.keys()) for edge_id in ingoing_edges])
            decremented_guards = set(str(dc.x) for edge in ingoing_edges for dc in dcp_edge_data[edge].constraints.values() if (dc.x == dc.y and dc.c < constants.ZERO))
            propagated_guards = shared_guards - decremented_guards
            
            if len(propagated_guards) == 0:
                continue
            
            # propagate to True branch
            for g in propagated_guards:
                guard_expression = guards_expressions[g].copy()
                dcp_edge_data[true_branch[0]].add_guard(guard_expression)
            
            
            if not dcp.is_back_edge(true_branch[0]):        
                # propagate in the loop body
                gaurd_propagation(dcp, dcp_edge_data, {true_branch[1]}, node)
            
            
            # try propagation from incomming edges including back edges to false branch    
            ingoing_edges = [edge_id for edge_id, _ in dcp.get_ingoing_edges(node)]
            guards_expressions = {g_str: g for edge_id in ingoing_edges for g_str,g in dcp_edge_data[edge_id].guards.items()}
            shared_guards = set.intersection(*[set(dcp_edge_data[edge_id].guards.keys()) for edge_id in ingoing_edges])
            decremented_guards = set(str(dc.x) for edge in ingoing_edges for dc in dcp_edge_data[edge].constraints.values() if (dc.x == dc.y and dc.c < constants.ZERO))
            propagated_guards = shared_guards - decremented_guards
            
            for g in propagated_guards:
                guard_expression = guards_expressions[g].copy()
                dcp_edge_data[false_branch[0]].add_guard(guard_expression)
        else:
            ingoing_edges = [edge_id for edge_id, _ in dcp.get_ingoing_edges(node)]
            guards_expressions = {g_str: g for edge_id in ingoing_edges for g_str,g in dcp_edge_data[edge_id].guards.items()}
            
            # operates on string  representation of the guards expressions
            # Intersetion of guard of ingoing edges - I
            shared_guards = set.intersection(*[set(dcp_edge_data[edge_id].guards.keys()) for edge_id in ingoing_edges])
            # Guards decremented on ingoing edges - D
            decremented_guards = set(str(dc.x) for edge in ingoing_edges for dc in dcp_edge_data[edge].constraints.values() if (dc.x == dc.y and dc.c < constants.ZERO))
            # Guards which are guaranteed to stay greater than zero - I \ D
            propagated_guards = shared_guards - decremented_guards
            
                
            # propagate guards and schedule propagation through targets
            if len(propagated_guards) > 0:
                for outgoing_edge, dst in dcp.get_outgoing_edges(node):
                    
                    if dst != loop_head_limit:
                        nodes_to_process.add(dst)
                    
                    for g in propagated_guards:
                        guard_expression = guards_expressions[g].copy()
                        dcp_edge_data[outgoing_edge].add_guard(guard_expression)
            
        
            
    
    


def dcp_to_guareded_dcp(dcp_edge_data) -> DCP:
    """ Transform regular dcp to DCP over natural numbers 
        
        e1 <= e2 + c, where c < 0 is transformed to
        1) [e1] <= [e2] - 1 if e2 is a guard
        2) [e1] <= [e2] otherwise
    """
        # Transformation to DCP over N
    analysis_logger.log(f"\tTransformation to N")
    for edge, label in dcp_edge_data.items():
        analysis_logger.log(f"\t Edge {edge} with guards {[g for g in label.guards.keys()]}")
        
        for dc in label.constraints.values():
            
            if dc.c < constants.ZERO:        # e1 <= e2 + c, c < 0
                
                if label.is_in_guards(dc.y): # e1 <= e2 - 1
                    new_dc = DC(dc.x.copy(), dc.y.copy(), -constants.ONE)
                    
                    analysis_logger.log(f"\t   - Transforming {str(dc)} into {str(new_dc)}")
                    dcp_edge_data[edge].add_dc(new_dc)  # replace difference cosntraint by e1 <= e2 - 1
                    
                else:                        # e1 <= e2 + 0
                    new_dc = DC(dc.x.copy(), dc.y.copy(), constants.ZERO)
                    analysis_logger.log(f"\t   - Transforming {str(dc)} into {str(new_dc)}")
                    dcp_edge_data[edge].add_dc(new_dc)  # replace difference cosntraint by e1 <= e2 - 1






@DCPConstructionWatch(watch=True)
def build_dcp(lts: LTS) -> DCP:
    analysis_logger.info("DCP Construction - []")
    dcp = DCP()
    lts_to_dcp(lts, dcp) # mapping graph strucuture of lts to dcp
    dcp, norms = infer_dcp_labels(lts, dcp) # inferring DCP transitions
    
    # dcp = dcp_to_guareded_dcp(dcp)
    analysis_logger.info("DCP Construction - [OK]")
    
    return dcp, norms