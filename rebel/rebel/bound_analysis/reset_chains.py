from reset_chain_graph import ResetChainGraph
from expression import Expression
from rebel.utils import constants
from .analysis_profiler import BoundAnalysisWatch
from collections import namedtuple
from pprint import pprint
from functools import reduce


type Atom = Expression
type Transition = int
type Constant = Expression
type AtomName = str
type ResetChainNode = tuple[Expression, Transition, Constant]

ResetChain = namedtuple('ResetChain', ['chain', 'trn', 'atm1', 'atm2', 'in_k', 'c'])

@BoundAnalysisWatch(watch=True)
def construct_reset_chain_graph(dcp) -> ResetChainGraph:
    reset_graph = ResetChainGraph()
    
    for edge in dcp.get_edges():
        for dc in dcp.get_edge_data(edge).constraints.values():
            # non-reset
            if dc.x == dc.y:          
                continue
            
            # pure constants reset - x <= c
            if dc.y == constants.ZERO:
                src = reset_graph.add_atom(dc.c.copy())
                dst = reset_graph.add_atom(dc.x.copy())
                reset_graph.add_edge(src, dst, (edge, constants.ZERO.copy()))
                continue
              
            # variables or program parameters
            src = reset_graph.add_atom(dc.y.copy())
            dst = reset_graph.add_atom(dc.x.copy())
            
            reset_graph.add_edge(src, dst, (edge, dc.c.copy()))

    return reset_graph

@BoundAnalysisWatch(watch=True)
def get_all_reset_chains(reset_graph, variables) -> dict[str, list[ResetChain]]:
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
            chain = forming_reset_chain.copy()
            chain.append((a_0, None, constants.ZERO.copy()))
            reset_chains[str(a_0)].append(chain)
            
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

@BoundAnalysisWatch(watch=True)
def get_optimal_reset_chains(dcp, reset_graph, variables):
    reset_chain_mapping = get_all_reset_chains(reset_graph, variables)
    
    for a_0, reset_chains in reset_chain_mapping.items():
        sound_optimal_chains = map(lambda c: obtain_optimal_chain(dcp, reset_graph, c), reset_chains)
        
        # TODO: better unique filtering of reset chains
        # remove duplicates
        unique_optimal_chains_hashes = {}
        unique_optimal_chains = []
        for c in sound_optimal_chains:
            new_chain = [(str(a), t) for a,t,_ in c.chain]
            h = reduce(lambda acc, x: acc + hash(x), new_chain, 0)
            
            unique_chain = unique_optimal_chains_hashes.get(h, None)
            if unique_chain and new_chain == unique_chain:
                continue
            
            unique_optimal_chains_hashes[h] = new_chain
            unique_optimal_chains.append(c)
            
            
        reset_chain_mapping[a_0] = unique_optimal_chains
    return reset_chain_mapping

@BoundAnalysisWatch(watch=True)            
def obtain_optimal_chain(dcp, reset_graph, reset_chain: ResetChain) -> ResetChain:
    """ Obtain the longest possible sound reset chain in the reset_chain """
    
    # NOTE: reset chain a_n -> a_n-1 -> ... -> a_1 -> a_0 is presented as
    # [(a_n, t_n, c_n), (a_n-1, t_n-1, c_n-1), ..., (a_1, t_1, c_1), (a_0, -1, 0)]
    
    # All reset chains of length 2 are sound
    if len(reset_chain) == 2: # a_1 -> a_0
        _in_k, t, _c = reset_chain[0]
        _atm1, _atm2 = {str(reset_chain[-1][0]): reset_chain[-1][0]}, {}
        _trn = {t}
        return ResetChain(reset_chain,
                        _trn,
                        _atm1,
                        _atm2,
                        _in_k,
                        _c)

    # Find optimal (longest) sound reset chain
    # Iteratively prepend a_i to the chain and check whether it is still sound
    # a_n -> a_n-1 -> ... -> a_2 -> a_1 -> a_0
    chain_start = len(reset_chain) - 3 # a_2
    while chain_start >= 0 and is_sound(dcp, reset_chain, chain_start):
        chain_start -= 1 # start is a_i+1, prepending atom to the chain
    
    # move back to the last start of sound reset chain 
    chain_start += 1
    
    # Obtain helper function results
    optimal_chain = reset_chain[chain_start:]
    
    
    # return a_{sound_start} -> ... -> a_0
    _atm1, _atm2 = atm_1_and_2(reset_graph, optimal_chain)
    _trn = trn(optimal_chain)
    _in_k = in_k(optimal_chain)
    _c = c(optimal_chain)
    return ResetChain(optimal_chain,
                      _trn,
                      _atm1,
                      _atm2,
                      _in_k,
                      _c)


# (a_i, l_1, l_2)
@BoundAnalysisWatch(watch=True)
def is_sound(dcp, reset_chain: ResetChain, chain_start: int) -> bool:
    # Check whether a_{i-1} is reset on all paths from target location of t_1 to source location of t_{i-1}
    # I.e. whether value of a_{i-1} is always known before the reset on t_{i-1}
    
    # a_0 : [(a_n, t_n, c_n), (a_n-1, t_n-1, c_n-1), ..., (a_1, t_1, c_1)]
    (a_i, t_i, _) = reset_chain[chain_start+1]
    (_, t_1, _) = reset_chain[-2]
    _, t_1_dst = dcp.get_edge_nodes(t_1)
    t_i_src, _ = dcp.get_edge_nodes(t_i)
    return atom_reset_on_all_paths(dcp, str(a_i), t_1_dst, t_i_src)
    
    
@BoundAnalysisWatch(watch=True)
def atom_reset_on_all_paths(dcp, atom: str, src: int, dst: int) -> bool:
    """ Check whether atom is reset on all paths from location src to dst
    """
    
    # Remove all the edges where atom is reset, atom <= y +c , y != atom
    # If dst is reachable from src, then there exists a path where atom
    # is not reset
    
    # NOTE: check validity of this adjustment
    if src == dst:
        return True
    
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


def number_of_paths_in_dag(graph, src, dst):
    dfs_stack = [src]
    n_paths = 0
    
    while len(dfs_stack) > 0:
        vertex = dfs_stack.pop()
        if vertex == dst:
            n_paths += 1
        else:
            for successor in graph.get_successors(vertex):
                dfs_stack.append(successor)
    return n_paths

def in_k(chain):
    atom, _, _ = chain[0]
    return atom

def atm(chain):
    atoms = {str(atom): atom for atom, _, _ in chain[1:]}
    return atoms

def atm_1_and_2(reset_graph, chain):
    atoms = atm(chain)
    atm1 = {}
    atm2 = {}
    
    chain_end_vertex_id = reset_graph.find_atom_vertex(str(chain[-1][0]))
    for atom_str, atom in atoms.items():
        atom_vertex_id = reset_graph.find_atom_vertex(atom_str)
        if number_of_paths_in_dag(reset_graph, atom_vertex_id, chain_end_vertex_id) > 1:
            atm2[atom_str] = atom
        else:
            atm1[atom_str] = atom
    
    return atm1, atm2

def trn(chain):
    return set([t for _,t,_ in chain if t is not None])

def c(chain):
    return sum([c for _,_,c in chain], start = constants.ZERO.copy())