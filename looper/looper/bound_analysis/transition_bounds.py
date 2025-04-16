""" Implementation of the bound algorithm """

from expression import Expression
from looper.utils import constants
from looper.utils import analysis_logger

from .analysis_profiler import BoundAnalysisWatch


@BoundAnalysisWatch(watch=True)
def transition_bound(dcp, local_bound_assignment, edge_id, result_cache) -> Expression:
    # Check whether result is in cache
    cache = result_cache['transition_bound'].get(edge_id, None)
    if cache != None:
        if cache[0] == 0: # Result of TB(t) is required to computed TB(t) -> infty bound
            analysis_logger.error(f"TransitionBound({edge_id}) - cyclic recursion")
            return None
        else:
            analysis_logger.log(f"\t\tTransitionBound({edge_id}) = {cache[1]} CACHED")
            return cache[1]
    result_cache['transition_bound'][edge_id] = (0, None)
    
    
    tb = __transition_bound(dcp, local_bound_assignment, edge_id, result_cache)
    
    if tb: # Finite reset sum
        result_cache['transition_bound'][edge_id] = (1, tb)
    
    analysis_logger.log(f"\t\tTransitionBound({edge_id}) = {str(tb)}")
    
    return tb


@BoundAnalysisWatch(watch=True)
def increment_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    # Check whether result is in cache
    cache = result_cache['increment_sum'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of Incr(t) is required to computed Incr(t) -> infty bound
            analysis_logger.error(f"IncrementSum({str(norm)}) - cyclic recursion")
            return None
        else:
            analysis_logger.log(f"\t\tIncrementSum({str(norm)}) = {cache[1]} CACHED")
            return cache[1]
    result_cache['increment_sum'][str(norm)] = (0, None)
    
    inc_sum = __increment_sum(dcp, local_bound_assignment, norm, result_cache)
    
    if inc_sum: # Finite reset sum
        result_cache['increment_sum'][str(norm)] = (1, inc_sum)
    
    analysis_logger.log(f"\t\tIncrementSum({str(norm)}) = {str(inc_sum)}")
    return inc_sum



@BoundAnalysisWatch(watch=True)
def reset_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    # Check whether result is in cache
    cache = result_cache['reset_sum'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of RS(n) is required to computed RS(n) -> infty bound
            analysis_logger.error(f"ResetSum({str(norm)}) - cyclic recursion")
            return None
        else:
            analysis_logger.log(f"\t\tResetSum({str(norm)}) = {cache[1]} CACHED")
            return cache[1]
    result_cache['reset_sum'][str(norm)] = (0, None)
    
    r_sum = __reset_sum(dcp, local_bound_assignment, norm, result_cache)
    
    if r_sum: # Finite reset sum
        result_cache['reset_sum'][str(norm)] = (1, r_sum)
        
    analysis_logger.log(f"\t\tResetSum({str(norm)}) = {str(r_sum)}")
    return r_sum


@BoundAnalysisWatch(watch=True)
def variable_bound(dcp, local_bound_assignment, norm, result_cache) -> Expression | None:
    # Check whether result is in cache
    cache = result_cache['variable_bound'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of VB(t) is required to computed VB(t) -> infty bound
            analysis_logger.error(f"VariableBound({str(norm)}) - cyclic recursion")
            return None
        else:
            analysis_logger.log(f"\t\tVariableBound({str(norm)}) = {cache[1]} CACHED")
            return cache[1]
    result_cache['variable_bound'][str(norm)] = (0, None)
    
    vb = __variable_bound(dcp, local_bound_assignment, norm, result_cache)

    if vb: # Finite reset sum
        result_cache['variable_bound'][str(norm)] = (1, vb)
        
    analysis_logger.log(f"\t\tVariableBound({str(norm)}) = {str(vb)}")
    return vb


def __transition_bound(dcp, local_bound_assignment, edge_id, result_cache) -> Expression:
    """ Return transition bound for a transition.
    
        TB(t) = LB(t)                                 if LB(t)  is build over constant
        TB(t) = IncrementSum(LB(t)) + ResetSum(LB(t)) if LB(t)  otherwise
    """
    analysis_logger.log(f"\t\tTransitionBound({edge_id}) = IncrementSum({local_bound_assignment[edge_id]}) + ResetSum({local_bound_assignment[edge_id]})")
    
    # LB(t)
    local_bound = local_bound_assignment[edge_id]
    
    # TB(t) = 1  for constant local bonds
    if local_bound == constants.ONE:
        result = constants.ONE.copy()
        return result
    
    
    # TB(t) = IncrementSum(LB(t)) + ResetSum(LB(t))
    _increment_sum = increment_sum(dcp, local_bound_assignment, local_bound, result_cache)
    _reset_sum = reset_sum(dcp, local_bound_assignment, local_bound, result_cache)
    
    if not _reset_sum or not _increment_sum: # cyclic recursion detection propagation, computation failed
        return None
    
    result = _increment_sum + _reset_sum 
    
    return result


def __increment_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    """ Return increment sum for a norm
        Incr(n) = sum [TB(t) * c]  for t where n is incremented by c
    """
    _increment_sum = Expression.create_constant(0)
    I = []
    inc_log = f"\t\tIncrementSum({str(norm)}) = "
    
    # Incr(n) = sum TB(t) * c
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)    
        constraint = label.constraints.get(str(norm), None)
        
        # x' <= x + c where c > 0
        if constraint and constraint.x == constraint.y and constraint.c > constants.ZERO:
            I.append((edge_id, constraint.c))
            inc_log += f"+ TB({edge_id}) x {constraint.c} "
    
    analysis_logger.log(inc_log)
    
    for edge, c in I:
        # TB(t)
        _transition_bound = transition_bound(dcp, local_bound_assignment, edge, result_cache)
        if not _transition_bound: # cyclic recursion detection propagation, computation failed
            return None

        # TB(t) * c
        _increment_sum += _transition_bound * c
    
    return _increment_sum


def __reset_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    """Return reset sum for a norm
       RS(n) = sum TB(t) * max(VB(n), 0)  for all t where n is reset
    """
        
    _reset_sum = Expression.create_constant(0)
    R = []
    res_log = f"\t\tResetSum({str(norm)}) = "
    # RS(n) = sum TB(t) * max(VB(n), 0)
    
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
         
        # x <= y + c where x != y
        if constraint and constraint.x != constraint.y:
            R.append((edge_id, constraint.y, constraint.c))
            res_log += f"+ TB({edge_id}) x max(VB({str(constraint.y)}) + {str(constraint.c)}, 0) "
    
    analysis_logger.log(res_log)

    for edge,a,c in R:
        # TB(t)
        _transition_bound = transition_bound(dcp, local_bound_assignment, edge, result_cache)
        if not _transition_bound: # cyclic recursion detection propagation, computation failed
            return None
        
        # VB(t)
        _variable_bound = variable_bound(dcp, local_bound_assignment, a, result_cache) # TODO change to max of resets
        if not _variable_bound: # cyclic recursion detection propagation, computation failed
            return None
        # VB(t) + c
        _variable_bound += c
        
        _reset_sum += _transition_bound * Expression.create_max([_variable_bound, constants.ZERO.copy()])
    
    
    return _reset_sum


def __variable_bound(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    """Return variable bound of a norm
        VB(e) = e                        if e is built over constants
        VB(e) = Incr(e) + max(VB(e) + c) otherwise
    """
    
    
    # VB(e), e built over constants
    # x <= 0 + c
    # x <= y + c where c is built over program parameters
    if norm == constants.ZERO or \
        set(norm.get_variable_names()).issubset(set((name for name,_ in dcp.get_parameters()))):
        return norm.copy()
    
    
    R = []
    vbs = []
    vb_log = f"\t\tVariableBound({str(norm)}) = IncrementSum({str(norm)}) + max("
      
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
        
        # is reset, x <= y + c, x!=y
        if constraint and constraint.x != constraint.y:
            R.append((edge_id, constraint.y, constraint.c))
            vb_log += f"+ VB({constraint.y} + {constraint.c}), "

    analysis_logger.log(vb_log + ")")

    for _, a, c in R:
        # VB(e_i)
        _variable_bound = variable_bound(dcp, local_bound_assignment, a, result_cache)
        if not _variable_bound: # cyclic recursion detection propagation, computation failed
            return None
        _variable_bound = _variable_bound.copy()
        
        # VB(e_i) + c_i
        _variable_bound += c
        
        vbs.append(_variable_bound)
    
    # Incr(e)
    _increment_sum = increment_sum(dcp, local_bound_assignment, norm, result_cache)
    if not _increment_sum: # cyclic recursion detection propagation, computation failed
        return None

    # Incr(e) + max(VB(e) + c)
    if len(vbs) == 0:
        return _increment_sum
    if len(vbs) == 1:
        return _increment_sum + vbs[0]
    
    return _increment_sum + Expression.create_max(vbs)
