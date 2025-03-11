""" Difference Constraint Program (DCP) construction """

from z3 import Int, And, Solver, unsat
from itertools import chain
from looper.profiling import ProfilingManager
from looper.utils import ZERO
from expression import Expression
from graphs import (DifferenceConstraintProgram as DCP,
                    DifferenceConstraint as DC,
                    DCPTransitionLabel,
                    LabeledTransitionSystem as LTS,
                    lts_to_dcp)

DCPConstructionWatch = ProfilingManager.watch('dcp_construction')



# TODO: helpers - should be encapsulated in DCP, LTS wrappers for Python
def get_transition_label(lts, edge):
    try:
        statements = lts.get_edge_data(edge).statements
    except ValueError:
        statements = None
    try:
        condition = lts.get_edge_data(edge).condition
        if (len(condition.condition) == 0):
            condition = None
    except ValueError:
        condition = None

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
        src, _ = lts.get_edge_nodes(edge)
        if not condition:   continue
        elif lts.is_loop_head(src) and condition.true_branch: # only true branch of the loop head
            # infer the norm from the condition
            # x - y for x > y
            # x - y + 1 for x >= y
            n = condition.get_norm()
            norms.append(n)
    # NOTE: unpack norms for now, disabling compound norms
    norms = [norm[0][0] for norm in norms]
    
    return norms
            
            
@DCPConstructionWatch(watch=True)
def execute_transition(norm: Norm, statements) -> Norm:
    """ Symbolicaly executes statements over a norm expression
        Returns resulting norm expression.
    """
    
    norm_copy = norm.copy()
    assignment_propagation = {}
    for stmt in statements:
        rhs = assignment_propagation.get(stmt.lhs, stmt.rhs)
        norm_copy.substitute(stmt.lhs, rhs)
        assignment_propagation[stmt.lhs] = rhs
    
    norm_copy.expand()
    return norm_copy


@DCPConstructionWatch(watch=True)
def infer_dcp_labels(lts: LTS, dcp: DCP) -> DCP:
    """ Label DCP by symbolically executing statements on the lts edge
        transitions and infer transition guards.
    """
    solver = Solver()
    stable_norm_set: list[Norm] = []
    norms_to_process: list[Norm] = initial_set_of_norms(lts)
    edges = list(filter(lambda x: x[1], [get_transition_label(lts, edge) for edge in lts.get_edges()]))
    
    dcp_edge_data = {edge: DCPTransitionLabel() for edge in lts.get_edges()}
    
    
    while len(norms_to_process) != 0:
        norm = norms_to_process.pop()
        stable_norm_set.append(norm)
        
        ctx = {v.replace('.', '_'):Int(v) for v in norm.get_variable_names()}
        
        
        for (edge, statements, condition) in edges:
            # 1) check whether it is a guard
            # if condition -> norm > 0 dcp_edge_data.add_guard(norm)
            if condition:
                
                
                ctx.update({v.replace('.', '_'):Int(v) for v in condition.get_norm()[0][0].get_variable_names()})
                
                z3_norm = eval(str(norm).replace('.', '_'), {}, ctx)
                z3_condition = eval(str(condition).replace('.', '_'), {}, ctx)
                formula = And(z3_condition, z3_norm <= 0)
                solver.reset()
                solver.add(formula)
                
                
                if solver.check() == unsat: # i.e.valid
                    dcp_edge_data[edge].add_guard(norm)
                
            
            # 2) Check whether a new norm was generated
            resulting_norm = execute_transition(norm, statements)
         
            for n in chain(stable_norm_set, norms_to_process):
                diff = resulting_norm - n
                        
                if diff.is_constant():
                    
                    if diff != ZERO or norm != n:
                        
                        dc = DC(norm, n, diff, False)
                        
                        dcp_edge_data[edge].add_dc(dc)
                        
                    break
            else:
                const = resulting_norm.separate_constant()
                c = Expression.create_constant(const)
                resulting_norm -= c        
                dc = DC(norm, resulting_norm, c, False)
                dcp_edge_data[edge].add_dc(dc)
                norms_to_process.append(resulting_norm)

    for edge, label in dcp_edge_data.items():
        dcp.set_edge_data(edge, label)
    
    # TODO guard propagation
    # for edge in dcp.get_edges():
    #     dcp_label = dcp.get_edge_data(edge)
    
    return dcp, stable_norm_set




@DCPConstructionWatch(watch=True)
def dcp_to_guareded_dcp(dcp: DCP) -> DCP:
    """ Transform regular dcp to DCP over natural numbers 
        
        e1 <= e2 + c, where c < 0 is transformed to
        1) [e1] <= [e2] - 1 if e2 is a guard
        2) [e1] <= [e2] otherwise
    """
    pass






@DCPConstructionWatch(watch=True)
def build_dcp(lts: LTS) -> DCP:
    dcp = DCP()
    lts_to_dcp(lts, dcp)
    dcp, norms = infer_dcp_labels(lts, dcp)

    # #dcp = dcp_to_guareded_dcp(dcp)
    
    return dcp, norms