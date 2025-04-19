from looper.config import Configuration
from looper.utils import analysis_logger
from expression import Expression
from variable_flow_graph import VariableFlowGraph as VFG
from difference_constraint_program import DifferenceConstraint as DC, DCPTransitionLabel


type Location = int
type VariableName = str

def construct_varaible_flow_graph(dcp, variables) -> VFG:
    
    # Create vertex (location, variable) for each loc, variable in the program
    # edges are added next
    # vertices with 0 ingoing or outgoing edges are removed
    vfg = VFG()
    dcp_start_location = dcp.get_start_location()
    
    for variable in variables:
        for vertex in dcp.get_nodes():
            if vertex != dcp_start_location:
                vfg.add_variable_vertex(vertex, variable)
    
    
    visited_vertices = set()
    dfs_stack = [v for v in dcp.get_successors(dcp_start_location)]
    
    while len(dfs_stack) > 0:
        vertex = dfs_stack.pop()
        visited_vertices.add(vertex)
        
        for successor in dcp.get_successors(vertex):

            # process edges between the two vertices
            for edge in dcp.get_edges_between(vertex, successor):
                
                for dc in dcp.get_edge_data(edge).constraints.values():
                    x,y = str(dc.x), str(dc.y)
                    
                    if y not in variables:
                        continue
                    
                    vfg_src_vertex = vfg.find_variable_vertex(vertex, y)
                    vfg_dst_vertex = vfg.find_variable_vertex(successor, x)
                    
                    if len(vfg.get_edges_between(vfg_src_vertex, vfg_dst_vertex)) > 0:
                        continue
                    
                    vfg.add_edge(vfg_src_vertex, vfg_dst_vertex)
                
            
            if successor not in visited_vertices and successor != dcp.get_end_location():
                dfs_stack.append(successor)
            
    
    return vfg

def construct_variable_renaming_mapping(vfg) -> dict[tuple[Location, VariableName], VariableName]:
    _sccs = vfg.sccs()
    return {vfg.get_node_data(vertex_id): f"var_{var_name}" for vertex_id, var_name in _sccs.items()}

def rename_variables(dcp, renaming_mapping: dict[tuple[Location, VariableName], VariableName]):
    for edge_id in dcp.get_edges():
        label = dcp.get_edge_data(edge_id)
        src, dst = dcp.get_edge_nodes(edge_id)
        constraints = label.constraints
        guards = label.guards
        
        renamed_label = DCPTransitionLabel()
        for dc in constraints.values():
            new_x_name = renaming_mapping.get((dst, str(dc.x)), None)
            new_y_name = renaming_mapping.get((src, str(dc.y)), None)
            # TODO: correctly solve variable definition for location
            # NOTE: patch for removal of x <= y + c on initial transition if y contains undefined variables
            if new_y_name == None and not set(dc.y.get_variable_names()).issubset(set((name for name,_ in dcp.get_parameters()))):
                continue
                
            renamed_x = Expression.create_variable(new_x_name) if new_x_name else dc.x.copy()
            renamed_y = Expression.create_variable(new_y_name) if new_y_name else dc.y.copy()
            c = dc.c.copy()
            renamed_label.add_dc(DC(renamed_x, renamed_y, c))
        
        
        for g in guards.values():
            new_guard_name = renaming_mapping.get((src, str(g)), None)
            renamed_guard = Expression.create_variable(new_guard_name) if new_guard_name else g.copy()
            renamed_label.add_guard(renamed_guard)

        dcp.set_edge_data(edge_id, renamed_label)
        
    return dcp

def flow_sensitive_transformation(dcp, variables):
    vfg = construct_varaible_flow_graph(dcp, variables)
    if Configuration.config['graphs']:
        analysis_logger.save_in_directory('vfg.dot', vfg.convert_to_dot())
        
    renaming_mapping = construct_variable_renaming_mapping(vfg)
    
    analysis_logger.info("Variable Renaming Mapping")
    for (loc, var), renamed_var in renaming_mapping.items():
        analysis_logger.log(f" - ({loc}, {var}) -> var_{renamed_var}")
        
    dcp = rename_variables(dcp, renaming_mapping)
    variables = [Expression.create_variable(var) for var in renaming_mapping.values()]
    
    return dcp, variables