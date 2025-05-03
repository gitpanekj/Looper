#include "labeled_transition_system/labeled_transition_system.hpp"
#include "labeled_transition_system/lts_labels.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <list>

namespace labeled_transition_system
{
    /* LTS */

    void LabeledTransitionSystem::add_parameter(std::string name, std::string type)
    {
        parameters.push_back({name, type});
        n_params++;
    }

    void LabeledTransitionSystem::add_parameters(std::vector<std::pair<std::string, std::string>> params)
    {
        for (auto param : params)
        {
            parameters.push_back(param);
            n_params++;
        }
    }

    void LabeledTransitionSystem::set_return_type(std::string name)
    {
        ret = name;
    }

    void LabeledTransitionSystem::set_function_name(std::string name)
    {
        fname = name;
    }

    int LabeledTransitionSystem::add_start_location()
    {
        start_location = this->add_vertex();
        return start_location;
    }

    int LabeledTransitionSystem::add_start_location(std::string data)
    {
        start_location = this->add_vertex(data);
        return start_location;
    }

    int LabeledTransitionSystem::add_end_location()
    {
        end_location = this->add_vertex();
        return end_location;
    }

    int LabeledTransitionSystem::add_end_location(std::string data)
    {
        end_location = this->add_vertex(data);
        return end_location;
    }

    void LabeledTransitionSystem::mark_loop_head(int vertex_id)
    {
        auto it = this->outgoing_edges.find(vertex_id);
        if (it == this->outgoing_edges.end())
            throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        loop_heads.insert(vertex_id);
    }

    void LabeledTransitionSystem::unmark_loop_head(int vertex_id)
    {
        auto it = this->outgoing_edges.find(vertex_id);
        if (it == this->outgoing_edges.end())
            throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        loop_heads.erase(vertex_id);
    }

    void LabeledTransitionSystem::mark_back_edge(int edge_id)
    {
        auto it = this->id_to_edge_mapping.find(edge_id);
        if (it == this->id_to_edge_mapping.end())
            throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        back_edges.insert(edge_id);
    }

    void LabeledTransitionSystem::unmark_back_edge(int edge_id)
    {
        auto it = this->id_to_edge_mapping.find(edge_id);
        if (it == this->id_to_edge_mapping.end())
            throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        back_edges.erase(edge_id);
    }

    bool LabeledTransitionSystem::is_loop_head(int vertex_id) const
    {
        auto it = this->outgoing_edges.find(vertex_id);
        if (it == this->outgoing_edges.end())
            throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        return loop_heads.find(vertex_id) != loop_heads.end();
    }
    bool LabeledTransitionSystem::is_back_edge(int edge_id) const
    {
        auto it = this->id_to_edge_mapping.find(edge_id);
        if (it == this->id_to_edge_mapping.end())
            throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        return back_edges.find(edge_id) != back_edges.end();
    };

    std::string parameters_string(const std::vector<std::pair<std::string, std::string>> &vec)
    {
        std::ostringstream dotStream;
        for (const auto &p : vec)
        {
            dotStream << p.first << ": " << p.second << ", ";
        }
        return dotStream.str();
    }

std::string LabeledTransitionSystem::convert_to_dot() const
    {
        std::ostringstream dotStream;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\tsubgraph cluster_title {" << std::endl;
        dotStream << "\t\tlabel=\"\";" << std::endl;
        dotStream << "\t\tstyle=invis;" << std::endl;
        dotStream << "\t\tnode [shape=plaintext];" << std::endl;
        dotStream << "\t\tTitleNode [label=" << "\"" << fname << "(" << parameters_string(parameters) << ")" << " -> " << ret
                  << "\"" << ",fontsize=12, fontcolor=black]" << std::endl;
        dotStream << "\t}" << std::endl;

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
                 try { label = this->edges_data.at(edge.id).to_string(); } catch (std::exception &e) {label = "";}
                auto color = is_back_edge(edge.id) ? "red" : "black";
                dotStream << "\t" << src << " -> " << edge.to 
                                  <<" [color=\"" << color << "\""
                                  << ",label=\"" << label <<  "\""
                                  << "];" 
                                  << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }
} // namespaces graphs