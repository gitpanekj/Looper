#include "graphs/labeled_transition_system.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <list>

namespace graphs
{
    /* LTS */
    template <typename NodeDataType, typename EdgeDataType>
    void LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_parameter(std::string name, std::string type)
    {
        parameters.push_back({name, type});
        n_params++;
    }
    
    template <typename NodeDataType, typename EdgeDataType>
    void LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_parameters(std::vector<std::pair<std::string, std::string>> parameters)
    {
        for (auto param : parameters)
        {
            parameters.push_back({param.first, param.second});
            n_params++;
        }
    }

    template <typename NodeDataType, typename EdgeDataType>
    void LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_ret(std::string name)
    {
        ret = name;
    }

    template <typename NodeDataType, typename EdgeDataType>
    void LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_name(std::string name)
    {
        fname = name;
    }

    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_start_location()
    {
        start_location = this->add_node();
        return start_location;
    }
    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_start_location(NodeDataType data)
    {
        start_location = this->add_node(data);
        return start_location;
    }

    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_end_location()
    {
        end_location = this->add_node();
        return end_location;
    }
    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_end_location(NodeDataType data)
    {
        end_location = this->add_node(data);
        return end_location;
    }

    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_location()
    {
        return this->add_node();
    }
    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_location(NodeDataType data)
    {
        return this->add_node(data);
    }

    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_transition(int src, int dst, EdgeDataType label)
    {
        return this->add_edge(src, dst, label);
    }
    template <typename NodeDataType, typename EdgeDataType>
    int LabeledTransitionSystem<NodeDataType, EdgeDataType>::add_transition(int src, int dst)
    {
        return this->add_edge(src, dst);
    }



    std::string parameters_string(const std::vector<std::pair<std::string, std::string>>& vec) {
        std::ostringstream dotStream;
        for (const auto& p : vec) {
            dotStream  << p.first << ": " << p.second << ", ";
        }
        return dotStream.str();
    }



    template <typename NodeDataType, typename EdgeDataType>
    std::string LabeledTransitionSystem<NodeDataType, EdgeDataType>::convert_to_dot()
    {
        std::ostringstream dotStream;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\tsubgraph cluster_title {" << std::endl;
        dotStream << "\t\tlabel=\"\";" << std::endl;
        dotStream << "\t\tstyle=invis;" << std::endl;
        dotStream << "\t\tnode [shape=plaintext];" << std::endl;
        dotStream << "\t\tTitleNode [label=" << "\"" << fname << "(" <<  parameters_string(parameters) << ")" << " -> " << ret
                  << "\"" << ",fontsize=12, fontcolor=black]" << std::endl;
        dotStream << "\t}" << std::endl;

        dotStream << "\t// Node definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            dotStream << "\t" << node.first << " [label=\"" << this->nodes[node.first] << "\"];" << std::endl;
        }

        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : this->outgoing_edges)
        {
            int src = node.first;
            for (const auto &edge : node.second)
            {
                dotStream << "\t" << src << " -> " << edge.to << " [label=\"" << this->edges[edge.id] << "\"]" << ";" << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }
} // namespaces graphs

// Templates
template class graphs::LabeledTransitionSystem<std::string, std::string>;