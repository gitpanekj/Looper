#include "graphs/reset_chain_graph.hpp"
#include "graphs/directed_labeled_graph.hpp"
#include "expression/expression.hpp"
#include <utility>
#include <memory>
#include <sstream>

namespace graphs
{
    int ResetChainGraph::find_atom_vertex(std::string atom)
    {
        auto it = vertex_label_to_vertex_id_mapping.find(atom);
        if (it == vertex_label_to_vertex_id_mapping.end()){return -1;}
        return it->second;
    }

    int ResetChainGraph::add_atom(Atom atom)
    {
        auto it = vertex_label_to_vertex_id_mapping.find(atom->to_string());

        // Vertex with id (location, variable) already exists
        if (it != vertex_label_to_vertex_id_mapping.end()){return it->second;}
        
        // Add vertex
        int vertex_id = ResetChainGraph::add_vertex(atom);

        // Update mapping
        vertex_label_to_vertex_id_mapping[atom->to_string()] = vertex_id;
        return vertex_id;
    }

    std::string ResetChainGraph::convert_to_dot() const
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
            std::shared_ptr<Expression> atom = this->vertices_data.at(node.first);
            dotStream << "\t" << node.first << " [label=\"" << "id: " << node.first << ", " << atom->to_string() << "\"];" << std::endl;
        }

        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            int src = node.first;
            for (const auto &edge : node.second)
            {
                const auto &[tau, c] = this->edges_data.at(edge.id);
                dotStream << "\t" << src << " -> " << edge.to 
                                  << "[label=\"" << "id:" << edge.id << ", tau=" << tau << ", c=" << c->to_string() <<  "\""
                                  << "];" 
                                  << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();

    }
}