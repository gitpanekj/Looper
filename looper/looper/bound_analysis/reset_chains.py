from graphs import ResetChainGraph
from expression import Expression
from looper.utils import constants

type Atom = Expression
type Transition = int
type Constant = Expression
type AtomName = str
type ResetChain = tuple[Expression, Transition, Constant]

def construct_reset_chain_graph(dcp, variables) -> ResetChainGraph:
    reset_graph = ResetChainGraph()
    
    # # Add node for each variable
    # # each variable must be reset in the program at least once (initial, reset)
    # for var in variables:
    #     reset_graph.add_atom(Expression.create_variable(var))

    # # Add constant program parameters
    # for param_name, _ in dcp.get_parameters():
    #     reset_graph.add_atom(Expression.create_variable(param_name))


    for edge in dcp.get_edges():
        for dc in dcp.get_edge_data(edge).constraints.values():
            # non-reset
            if dc.x == dc.y:          
                continue
            
            # pure constants reset - x <= c
            if dc.y == constants.ZERO:
                src = reset_graph.add_atom(dc.c.copy())
                #dst = reset_graph.find_atom_vertex(str(dc.))
                dst = reset_graph.add_atom(dc.x.copy())
                reset_graph.add_edge(src, dst, (edge, constants.ZERO.copy()))
                continue
              
            # variables or program parameters
            #src = reset_graph.find_atom_vertex(str(dc.y))
            src = reset_graph.add_atom(dc.y.copy())
            #dst = reset_graph.find_atom_vertex(str(dc.x))
            dst = reset_graph.add_atom(dc.x.copy())
            
            reset_graph.add_edge(src, dst, (edge, dc.c.copy()))

    return reset_graph

def get_all_reset_chains(dcp, reset_graph, variables) -> dict[str, list[ResetChain]]:
    reset_chains = {var: [] for var in variables}
    
    source_nodes = [node for node in reset_graph.get_nodes() if len(reset_graph.get_predecessors(node)) == 0]

    UNPROCESSED = 0
    PROCESSED = 1
    
    # (node_id, outgoing_edge_id, status)
    dfs_stack = [(node, outgoing_edge, target, UNPROCESSED) for node in source_nodes for (outgoing_edge, target) in reset_graph.get_outgoing_edges(node)]
    forming_reset_chain = []
    
    while len(dfs_stack) > 0:
        node, outgoing_edge, target, status = dfs_stack.pop()
        
        if status == PROCESSED: # all the otugoing edges processed
            # Generate new reset chain
            a_0 = reset_graph.get_node_data(target)
            reset_chains[str(a_0)].append(forming_reset_chain.copy())
            
            # Pop from the reset chain
            forming_reset_chain.pop()
            
        else:
            # place back to the stack
            dfs_stack.append((node, outgoing_edge, target, PROCESSED))
            
            # update append to the reset chain
            atom = reset_graph.get_node_data(node)
            t, c = reset_graph.get_edge_data(outgoing_edge)
            forming_reset_chain.append((atom, t, c))
            
            # processing successor
            successor_outgoing_paths = [(target, succ_outgoing_edge,succ_target, UNPROCESSED) for succ_outgoing_edge, succ_target in reset_graph.get_outgoing_edges(target)]
            dfs_stack.extend(successor_outgoing_paths)
    
    return reset_chains 


def get_optimal_reset_chains(dcp, reset_graph, variables):
    reset_chain_mapping = get_all_reset_chains(dcp, reset_graph, variables)

    
    for a_0, reset_chains in reset_chain_mapping.items():
        sound_optimal_chains = map(lambda c: obtain_optimal_chain(dcp, c), reset_chains)
        reset_chain_mapping[a_0] = list(sound_optimal_chains)
    
    return reset_chain_mapping

            
def obtain_optimal_chain(dcp, reset_chain: ResetChain) -> ResetChain:
    """ Obtain the longest possible sound reset chain in the reset_chain """
    
    # NOTE: reset chain a_n -> a_n-1 -> ... -> a_1 -> a_0 is presented as
    # [(a_n, t_n, c_n), (a_n-1, t_n-1, c_n-1), ..., (a_1, t_1, c_1)]
    
    # All reset chains of length 2 are sound
    if len(reset_chain) == 1: # a_1 -> a_0
        return reset_chain

    # Find optimal (longest) sound reset chain
    # Iteratively prepend a_i to the chain and check whether it is still sound
    # a_n -> a_n-1 -> ... -> a_2 -> a_1 -> a_0
    chain_start = len(reset_chain) - 2 # a_2
    while chain_start >= 0 and is_sound(dcp, reset_chain, chain_start):
        chain_start -= 1 # start is a_i+1, prepending atom to the chain
    
    # move back to the last start of sound reset chain 
    chain_start += 1
    
    # return a_{sound_start} -> ... -> a_0
    return reset_chain[chain_start:]


# (a_i, l_1, l_2)
def is_sound(dcp, reset_chain: ResetChain, chain_start: int) -> bool:
    # Check whether a_{i-1} is reset on all paths from target location of t_1 to source location of t_{i-1}
    # I.e. whether value of a_{i-1} is always known before the reset on t_{i-1}
    
    # a_0 : [(a_n, t_n, c_n), (a_n-1, t_n-1, c_n-1), ..., (a_1, t_1, c_1)]
    (a_i, t_i, _) = reset_chain[chain_start+1]
    (_, t_1, _) = reset_chain[len(reset_chain)-1]
    _, t_1_dst = dcp.get_edge_nodes(t_1)
    t_i_src, _ = dcp.get_edge_nodes(t_i)
    return atom_reset_on_all_paths(dcp, str(a_i), t_1_dst, t_i_src)
    
    

def atom_reset_on_all_paths(dcp, atom: str, src: int, dst: int) -> bool:
    """ Check whether atom is reset on all paths from location src to dst
    """
    
    # Remove all the edges where atom is reset, atom <= y +c , y != atom
    # If dst is reachable from src, then there exists a path where atom
    # is not reset
    
    # Reset set
    R = []
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        constraint = label.constraints.get(atom, None) 
        
        # x <= y + c where x != y
        if constraint and constraint.x != constraint.y:
            R.append(edge_id)
    
    # Invalidate the edges
    for edge_id in R:
        dcp.mark_as_erased(edge_id)
    
    # Check reachability
    dst_reachable_from_src = is_reachable_from(dcp, src, dst)
    
    # Validate the edges back
    for edge_id in R:
        dcp.unmark_as_erased(edge_id)
    
    return not dst_reachable_from_src
    

def is_reachable_from(dcp, src: int, dst: int) -> bool:
    dfs_stack = [src]
    visited_vertices = set()
    
    while len(dfs_stack) > 0:
        vertex = dfs_stack.pop()
        if vertex == dst:
            return True
        
        visited_vertices.add(vertex)
        
        for succesor in dcp.get_successors(vertex):
            if succesor not in visited_vertices:
                dfs_stack.append(succesor)
        
    return False