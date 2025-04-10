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
            analysis_logger.log(f"\t TransitionBound({edge_id}) - cyclic recursion")
            return None
        else:
            return cache[1]
    result_cache['transition_bound'][edge_id] = (0, None)
    
    
    tb = __transition_bound(dcp, local_bound_assignment, edge_id, result_cache)
    
    
    if tb: # Finite reset sum
        result_cache['transition_bound'][edge_id] = (1, tb)
    
    return tb


@BoundAnalysisWatch(watch=True)
def increment_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    # Check whether result is in cache
    cache = result_cache['increment_sum'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of Incr(t) is required to computed Incr(t) -> infty bound
            analysis_logger.log(f"\t IncrementSum({str(norm)}) - cyclic recursion")
            return None
        else:
            return cache[1]
    result_cache['increment_sum'][str(norm)] = (0, None)
    
    inc_sum = __increment_sum(dcp, local_bound_assignment, norm, result_cache)
    
    if inc_sum: # Finite reset sum
        result_cache['increment_sum'][str(norm)] = (1, inc_sum)
    
    return inc_sum



@BoundAnalysisWatch(watch=True)
def reset_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    # Check whether result is in cache
    cache = result_cache['reset_sum'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of RS(n) is required to computed RS(n) -> infty bound
            analysis_logger.log(f"\t ResetSum({str(norm)}) - cyclic recursion")
            return None
        else:
            return cache[1]
    result_cache['reset_sum'][str(norm)] = (0, None)
    
    r_sum = __reset_sum(dcp, local_bound_assignment, norm, result_cache)
    
    if r_sum: # Finite reset sum
        result_cache['reset_sum'][str(norm)] = (1, r_sum)
        
    return r_sum


@BoundAnalysisWatch(watch=True)
def variable_bound(dcp, local_bound_assignment, norm, result_cache) -> Expression | None:
    # Check whether result is in cache
    cache = result_cache['variable_bound'].get(str(norm), None)
    if cache != None:
        if cache[0] == 0: # Result of VB(t) is required to computed VB(t) -> infty bound
            analysis_logger.log(f"\t VariableBound({str(norm)}) - cyclic recursion")
            return None
        else:
            return cache[1]
    result_cache['variable_bound'][str(norm)] = (0, None)
    
    vb = __variable_bound(dcp, local_bound_assignment, norm, result_cache)

    if vb: # Finite reset sum
        result_cache['variable_bound'][str(norm)] = (1, vb)
        
    return vb


def __transition_bound(dcp, local_bound_assignment, edge_id, result_cache) -> Expression:
    """ Return transition bound for a transition.
    
        TB(t) = LB(t)                                 if LB(t)  is build over constant
        TB(t) = IncrementSum(LB(t)) + ResetSum(LB(t)) if LB(t)  otherwise
    """
    
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
    
    # Incr(n) = sum TB(t) * c
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)    
        constraint = label.constraints.get(str(norm), None)
        
        # x' <= x + c where c > 0
        if constraint and constraint.x == constraint.y and constraint.c > constants.ZERO:
            # TB(t)
            _transition_bound = transition_bound(dcp, local_bound_assignment, edge_id, result_cache)
            if not _transition_bound: # cyclic recursion detection propagation, computation failed
                return None

            _increment_sum += constraint.c * _transition_bound
    
    return _increment_sum


def __reset_sum(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    """Return reset sum for a norm
       RS(n) = sum TB(t) * max(VB(n), 0)  for all t where n is reset
    """
        
    _reset_sum = Expression.create_constant(0)
    
    # RS(n) = sum TB(t) * max(VB(n), 0)
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
        
        # x <= y + c where x != y
        if constraint and constraint.x != constraint.y:
            
            # TB(t)
            _transition_bound = transition_bound(dcp, local_bound_assignment, edge_id, result_cache)
            if not _transition_bound: # cyclic recursion detection propagation, computation failed
                return None
            
            # VB(t)
            _variable_bound = variable_bound(dcp, local_bound_assignment, constraint.y, result_cache) # TODO change to max of resets
            if not _variable_bound: # cyclic recursion detection propagation, computation failed
                return None
            # VB(t) + c
            _variable_bound += constraint.c
            
            _reset_sum += _transition_bound * _variable_bound

    
    return _reset_sum


def __variable_bound(dcp, local_bound_assignment, norm, result_cache) -> Expression:
    """Return variable bound of a norm
        VB(e) = e                        if e is built over constants
        VB(e) = Incr(e) + max(VB(e) + c) otherwise
    """
    
    # TODO: check if it is built over program parameters, then return
    
    resets = []
      
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(str(norm), None)
        
        # is reset, x <= y + c, x!=y
        if constraint and constraint.x != constraint.y:
            # VB(e_i)
            _variable_bound = variable_bound(dcp, local_bound_assignment, constraint.y, result_cache)
            if not _variable_bound: # cyclic recursion detection propagation, computation failed
                return None
            
            # VB(e_i) + c_i
            _variable_bound += constraint.c
            
            resets.append(_variable_bound)
    
    
    if not resets: # built over format parameters
        result_cache['variable_bound'][str(norm)] = (1, norm) 
        return norm
    
    # Incr(e)
    _increment_sum = increment_sum(dcp, local_bound_assignment, norm, result_cache)
    if not _increment_sum: # cyclic recursion detection propagation, computation failed
        return None

    # Incr(e) + max(VB(e) + c)
    return _increment_sum + resets[0]  # TODO: change to max of resets
