from .analysis_profiler import BoundAnalysisWatch
from .transition_bounds import transition_bound

from looper.utils import analysis_logger
from expression import Expression


@BoundAnalysisWatch(watch=True)
def cost_bounds(dcp, local_bound_mapping) -> Expression | None:
    result_cache = {
        'transition_bound': {},
        'increment_sum': {},
        'reset_sum': {},
        'variable_bound': {},
    }
    
    back_edges = dcp.get_back_edges()
    total_bound = Expression.create_constant(0)
    
    analysis_logger.info("Bound analysis")
    
    # Cost = sum TransitionBound(t) x TranstionCost(t) 
    for back_edge in back_edges:
        analysis_logger.log(f"\tEdgeCost({back_edge})")
        
        # TB(t)
        tb = transition_bound(dcp, local_bound_mapping, back_edge, result_cache)
        if not tb: # Infinte transitio bound
            total_bound = None
            break
        
        total_bound += tb

    analysis_logger.log(f"\t[{'OK' if total_bound else 'FAILED':^6}] Cost = {total_bound}")
    
    return total_bound