#include "variable_flow_graph/variable_flow_graph.hpp"
#include "directed_labeled_graph/directed_labeled_graph.hpp"
#include <utility>
#include <iostream>
#include <string>
#include <sstream>


namespace variable_flow_graph
{
    using VertexDataType = std::pair<Location, VarName>; 
    using EdgeDataType = std::string;
    using DLG = directed_labeled_graph::DirectedLabeledGraph<VertexDataType, EdgeDataType>;
    

    int VariableFlowGraph::add_variable_vertex(int location, std::string variable)
    {
        auto it = vertex_label_to_vertex_id_mapping.find({location, variable});
        
        // Vertex with id (location, variable) already exists
        if (it != vertex_label_to_vertex_id_mapping.end()){return it->second;}
        
        // Add vertex
        int vertex_id = DLG::add_vertex({location, variable});

        // Update mapping
        vertex_label_to_vertex_id_mapping[{location, variable}] = vertex_id;
        return vertex_id;
    }


    void VariableFlowGraph::delete_variable_vertex(int location, std::string variable)
    {
        auto it = vertex_label_to_vertex_id_mapping.find({location, variable});
        
        // No vertex with id (location, variable)
        if (it == vertex_label_to_vertex_id_mapping.end()){return;}
        
        // Update mapping
        vertex_label_to_vertex_id_mapping.erase({location, variable});

        // Remove vertex
        int vertex_id = it->second;
        DLG::delete_vertex(vertex_id);
    }

    int VariableFlowGraph::find_variable_vertex(int location, std::string variable)
    {
        auto it = vertex_label_to_vertex_id_mapping.find({location, variable});
        
        // Vertex with id (location, variable) already exists
        if (it != vertex_label_to_vertex_id_mapping.end()){return it->second;}
        
        return -1;
    }

    std::string VariableFlowGraph::convert_to_dot() const
    {
        std::ostringstream dotStream;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\tsubgraph cluster_title {" << std::endl;
        dotStream << "\t\tlabel=\"\";" << std::endl;
        dotStream << "\t\tstyle=invis;" << std::endl;
        dotStream << "\t\tnode [shape=plaintext]; }" << std::endl;

        dotStream << "\t// Node definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            const auto &[vertex_id, variable] = this->vertices_data.at(node.first);
            dotStream << "\t" << node.first << " [label=\"" << "(" << vertex_id << ", " << variable << ")" << "\"];" << std::endl;
        }

        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            int src = node.first;
            for (const auto &edge : node.second)
            {
                dotStream << "\t" << src << " -> " << edge.to << ";" << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }
}