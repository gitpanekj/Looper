/**
 * @file directed_labeled_graph.cpp
 * @author Jan Pánek xpanek11@stud.fit.vutbr.cz
 * @brief Implementation of directed labeled graph
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "graphs/directed_labeled_graph.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>

namespace graphs
{

    /* DirectedLabeledGraph implementation */

    /* Beginning of node operations */
    template <typename NodeDataType, typename EdgeDataType>
    int DirectedLabeledGraph<NodeDataType, EdgeDataType>::add_node()
    {
        // Update id_counter
        int node_id = node_id_counter++;

        // Update adjacency list
        outgoing_edges[node_id] = std::list<OutgoingEdge>();
        ingoing_edges[node_id] = std::list<IngoingEdge>();

        return node_id;
    };

    template <typename NodeDataType, typename EdgeDataType>
    int DirectedLabeledGraph<NodeDataType, EdgeDataType>::add_node(NodeDataType node)
    {
        // Update id_counter
        int node_id = node_id_counter++;

        // Update adjacency list
        outgoing_edges[node_id] = std::list<OutgoingEdge>();
        ingoing_edges[node_id] = std::list<IngoingEdge>();

        // Add node data
        nodes[node_id] = node;

        return node_id;
    };

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::delete_node(int node_id)
    {
        // remove from adjacency lists
        outgoing_edges.erase(node_id);
        ingoing_edges.erase(node_id);
        // erase node data
        nodes.erase(node_id);

        // NOTE: there may be multiple edges from src to dst
        // outgoing edges
        for (auto &src_node : outgoing_edges)
        {
            std::list<OutgoingEdge> &edges_from_src = src_node.second;
            for (auto it = edges_from_src.begin(); it != edges_from_src.end();)
            {
                if (it->to == node_id)
                {
                    id_to_edge_mapping.erase(it->id);
                    it = edges_from_src.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }
        // ingoing edges
        for (auto &dst_node : ingoing_edges)
        {
            std::list<IngoingEdge> &edges_to_dst = dst_node.second;
            for (auto it = edges_to_dst.begin(); it != edges_to_dst.end();)
            {
                if (it->from == node_id)
                {
                    // NOTE: id_to_edge_mapping.erase(it->id); removed in the previous loop
                    it = edges_to_dst.erase(it);
                }
                else
                {
                    it++;
                }
            }
        }
    };

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::set_node_data(int node_id, NodeDataType node)
    {
        nodes[node_id] = node;
    }

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::reset_node_data(int node_id)
    {
        nodes.erase(node_id);
    }

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::update_node(int node_id, std::function<void(NodeDataType &node)> update_function)
    {
        NodeDataType &node = nodes[node_id];
        update_function(node);
    }

    template <typename NodeDataType, typename EdgeDataType>
    const NodeDataType &DirectedLabeledGraph<NodeDataType, EdgeDataType>::get_node_data(int node_id)
    {
        return nodes.at(node_id);
    }

    template <typename NodeDataType, typename EdgeDataType>
    std::vector<int> DirectedLabeledGraph<NodeDataType, EdgeDataType>::get_edges_between(int src, int dst)
    {
        std::vector<int> edges_between_src_dst;
        for (const auto &edge : outgoing_edges[src])
        {
            if (edge.to == dst)
            {
                edges_between_src_dst.push_back(edge.id);
            }
        }

        return edges_between_src_dst;
    }
    /* End of node operations */

    /* Beginning of edge operations */
    template <typename NodeDataType, typename EdgeDataType>
    int DirectedLabeledGraph<NodeDataType, EdgeDataType>::add_edge(int src, int dst, EdgeDataType label)
    {
        int edge_id = edge_id_counter++;
        id_to_edge_mapping[edge_id] = std::tuple(src, dst);
        outgoing_edges[src].emplace_back(dst, edge_id);
        ingoing_edges[dst].emplace_back(src, edge_id);
        edges[edge_id] = label;
        return edge_id;
    }

    template <typename NodeDataType, typename EdgeDataType>
    int DirectedLabeledGraph<NodeDataType, EdgeDataType>::add_edge(int src, int dst)
    {
        int edge_id = edge_id_counter++;
        id_to_edge_mapping[edge_id] = std::tuple(src, dst);
        outgoing_edges[src].emplace_back(dst, edge_id);
        ingoing_edges[dst].emplace_back(src, edge_id);
        return edge_id;
    }

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::delete_edge(int edge_id)
    {
        std::tuple<int, int> edge = id_to_edge_mapping[edge_id];
        int src = std::get<0>(edge);
        int dst = std::get<1>(edge);

        outgoing_edges[src].remove_if([edge_id](const OutgoingEdge &edge)
                                      { return edge.id == edge_id; });

        ingoing_edges[dst].remove_if([edge_id](const IngoingEdge &edge)
                                     { return edge.id == edge_id; });
        id_to_edge_mapping.erase(edge_id);
        edges.erase(edge_id);
    };

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::set_edge_data(int edge_id, EdgeDataType edge)
    {
        edges[edge_id] = edge;
    }

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::reset_edge_data(int edge_id)
    {
        edges.erase(edge_id);
    }

    template <typename NodeDataType, typename EdgeDataType>
    const EdgeDataType &DirectedLabeledGraph<NodeDataType, EdgeDataType>::get_edge_data(int edge_id)
    {
        return edges.at(edge_id);
    }

    template <typename NodeDataType, typename EdgeDataType>
    std::tuple<int, int> DirectedLabeledGraph<NodeDataType, EdgeDataType>::get_edge_nodes(int edge_id)
    {
        return id_to_edge_mapping[edge_id];
    }

    template <typename NodeDataType, typename EdgeDataType>
    void DirectedLabeledGraph<NodeDataType, EdgeDataType>::update_edge(int edge_id, std::function<void(EdgeDataType &edge)> update_function)
    {
        EdgeDataType &edge = edges[edge_id];
        update_function(edge);
    };

    /* End of edge operations */

    template <typename NodeDataType, typename EdgeDataType>
    std::string DirectedLabeledGraph<NodeDataType, EdgeDataType>::convert_to_dot()
    {
        std::ostringstream dotStream;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\t// Node definitions " << std::endl;
        for (auto &node : outgoing_edges)
        {
            dotStream << "\t" << node.first << " [label=\"" << nodes[node.first] << "\"];" << std::endl;
        }
        
        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : outgoing_edges)
        {
            int src = node.first;
            std::list<OutgoingEdge> &edges_from_src = node.second;
            for (const auto &edge : edges_from_src)
            {
                dotStream << "\t" << src << " -> " << edge.to << " [label=\"" << edges[edge.id] << "\"]" << ";" << std::endl;
            }
        }

        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }

    /* End of DirectedLabeledGraph */
}
template class graphs::DirectedLabeledGraph<std::string, std::string>;