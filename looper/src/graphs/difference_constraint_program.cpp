#include "graphs/difference_constraint_program.hpp"
#include <iostream>
#include <string>
#include <sstream>

namespace graphs
{
    bool DifferenceConstraintProgram::is_loop_head(int vertex_id) const
    {
        auto it = this->outgoing_edges.find(vertex_id);
        if (it == this->outgoing_edges.end())
            throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        return loop_heads.find(vertex_id) != loop_heads.end();
    }

    bool DifferenceConstraintProgram::is_back_edge(int edge_id) const
    {
        auto it = this->id_to_edge_mapping.find(edge_id);
        if (it == this->id_to_edge_mapping.end())
            throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        return back_edges.find(edge_id) != back_edges.end();
    };

    std::string DifferenceConstraintProgram::convert_to_dot() const
    {
        std::ostringstream dotStream;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\t// Node definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            auto shape = is_loop_head(node.first) ? "invhouse" : "ellipse";
            dotStream << "\t" << node.first << " [label=\"" << this->vertices_data.at(node.first) << "\",shape=" << shape << "];" << std::endl;
        }

        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            int src = node.first;
            for (const auto &edge : node.second)
            {
                std::string label;
                try
                {
                    label = this->edges_data.at(edge.id).to_string();
                }
                catch (std::exception &e)
                {
                    label = "";
                }
                auto color = is_back_edge(edge.id) ? "red" : "black";
                dotStream << "\t" << src << " -> " << edge.to
                          << " [color=\"" << color << "\""
                          << ",label=\"" <<  edge.id << " : " << label << "\""
                          << ",minlen=3"
                          << "];"
                          << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }
} // namespaces graphs