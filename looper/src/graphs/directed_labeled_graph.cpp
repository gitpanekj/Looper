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
#include "graphs/lts_labels.hpp"
#include "graphs/dcp_labels.hpp"
#include "expression/expression.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <tuple>
#include <optional>
#include <utility>
#include <memory>

namespace graphs
{

    /* DirectedLabeledGraph implementation */

    /* Beginning of node operations */
    template <typename VertexDataType, typename EdgeDataType>
    int DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_vertex()
    {
        // Update id_counter
        int vertex_id = vertex_id_counter++;

        // Update adjacency list
        outgoing_edges[vertex_id] = std::list<OutgoingEdge>();
        ingoing_edges[vertex_id] = std::list<IngoingEdge>();

        return vertex_id;
    };

    template <typename VertexDataType, typename EdgeDataType>
    int DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_vertex(VertexDataType node)
    {
        // Update id_counter
        int vertex_id = vertex_id_counter++;

        // Update adjacency list
        outgoing_edges[vertex_id] = std::list<OutgoingEdge>();
        ingoing_edges[vertex_id] = std::list<IngoingEdge>();

        // Add node data
        vertices_data[vertex_id] = node;

        return vertex_id;
    };

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::delete_vertex(int vertex_id)
    {
        // remove from adjacency lists
        auto it = outgoing_edges.erase(vertex_id);
        if (it == 0) throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        ingoing_edges.erase(vertex_id);
        // erase node data
        vertices_data.erase(vertex_id);

        // NOTE: there may be multiple edges from src to dst
        // outgoing edges
        for (auto &src_node : outgoing_edges)
        {
            std::list<OutgoingEdge> &edges_from_src = src_node.second;
            for (auto it = edges_from_src.begin(); it != edges_from_src.end();)
            {
                if (it->to == vertex_id)
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
                if (it->from == vertex_id)
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

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::set_vertex_data(int vertex_id, VertexDataType data)
    {
        auto it = outgoing_edges.find(vertex_id);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        vertices_data[vertex_id] = data;
    }

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::clear_vertex_data(int vertex_id)
    {
        auto it = outgoing_edges.find(vertex_id);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        vertices_data.erase(vertex_id);
    }

    template <typename VertexDataType, typename EdgeDataType>
    VertexDataType &DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_vertex_data(int vertex_id)
    {
        auto it = outgoing_edges.find(vertex_id);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(vertex_id) + " does not identify a vertex.");
        auto data = vertices_data.find(vertex_id);
        if (data == vertices_data.end()) throw std::invalid_argument("No data is associated with vertex" + std::to_string(vertex_id) + ".");
        return data->second;
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::vector<int> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edges_between(int src, int dst) const
    {
        auto it = outgoing_edges.find(src);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(src) + " does not identify a vertex.");
        it = outgoing_edges.find(dst);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(dst) + " does not identify a vertex.");

        std::vector<int> edges_between_src_dst;
        for (const auto &edge : outgoing_edges.at(src))
        {
            if (edge.to == dst and erased_edges.find(edge.id) == erased_edges.end())
            {
                edges_between_src_dst.push_back(edge.id);
            }
        }
        return edges_between_src_dst;
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::vector<std::tuple<int, int>> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_outgoing_edges(int src) const
    {
        auto it = outgoing_edges.find(src);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(src) + " does not identify a vertex.");

        std::vector<std::tuple<int, int>> __outgoing_edges;
        for (const auto &edge : outgoing_edges.at(src))
        {
            if (erased_edges.find(edge.id) != erased_edges.end()) continue;
            __outgoing_edges.push_back({edge.id, edge.to});
        }
        return __outgoing_edges;
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::vector<std::tuple<int, int>> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_ingoing_edges(int dst) const
    {
        auto it = outgoing_edges.find(dst);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(dst) + " does not identify a vertex.");

        std::vector<std::tuple<int, int>> __ingoing_edges;
        for (const auto &edge : ingoing_edges.at(dst))
        {
            if (erased_edges.find(edge.id) != erased_edges.end()) continue;
            __ingoing_edges.push_back({edge.id, edge.from});
        }
        return __ingoing_edges;
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::vector<int> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_successors(int src) const
    {

        auto it = outgoing_edges.find(src);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(src) + " does not identify a vertex.");

        std::unordered_set<int> successors;
        for (const auto &edge : outgoing_edges.at(src))
        {
            if (erased_edges.find(edge.id) != erased_edges.end()) continue;
            successors.insert(edge.to);
        }
        return std::vector<int>(successors.begin(), successors.end());
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::vector<int> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_predecessors(int dst) const
    {
        auto it = outgoing_edges.find(dst);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(dst) + " does not identify a vertex.");

        std::unordered_set<int> predecessors;
        for (const auto &edge : ingoing_edges.at(dst))
        {
            if (erased_edges.find(edge.id) != erased_edges.end()) continue;
            predecessors.insert(edge.from);
        }
        return std::vector<int>(predecessors.begin(), predecessors.end());
    }
    /* End of node operations */

    /* Beginning of edge operations */
    template <typename VertexDataType, typename EdgeDataType>
    int DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_edge(int src, int dst, EdgeDataType label)
    {
        auto it = outgoing_edges.find(src);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(src) + " does not identify a vertex.");
        it = outgoing_edges.find(dst);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(dst) + " does not identify a vertex.");

        int edge_id = edge_id_counter++;
        id_to_edge_mapping[edge_id] = std::tuple<int, int>(src, dst);
        outgoing_edges[src].emplace_back(dst, edge_id);
        ingoing_edges[dst].emplace_back(src, edge_id);
        edges_data[edge_id] = label;
        return edge_id;
    }

    template <typename VertexDataType, typename EdgeDataType>
    int DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_edge(int src, int dst)
    {
        auto it = outgoing_edges.find(src);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(src) + " does not identify a vertex.");
        it = outgoing_edges.find(dst);
        if (it == outgoing_edges.end()) throw std::invalid_argument("Given vertex_id " + std::to_string(dst) + " does not identify a vertex.");

        int edge_id = edge_id_counter++;
        id_to_edge_mapping[edge_id] = std::tuple<int, int>(src, dst);
        outgoing_edges[src].emplace_back(dst, edge_id);
        ingoing_edges[dst].emplace_back(src, edge_id);
        return edge_id;
    }

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::delete_edge(int edge_id)
    {
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == 0) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");

        std::tuple<int, int> edge = id_to_edge_mapping[edge_id];
        int src = std::get<0>(edge);
        int dst = std::get<1>(edge);

        outgoing_edges[src].remove_if([edge_id](const OutgoingEdge &edge)
                                      { return edge.id == edge_id; });

        ingoing_edges[dst].remove_if([edge_id](const IngoingEdge &edge)
                                     { return edge.id == edge_id; });
        id_to_edge_mapping.erase(edge_id);
        edges_data.erase(edge_id);
    };

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::set_edge_data(int edge_id, EdgeDataType edge)
    {
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        edges_data[edge_id] = edge;
    }

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::clear_edge_data(int edge_id)
    {
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        edges_data.erase(edge_id);
    }

    template <typename VertexDataType, typename EdgeDataType>
    EdgeDataType &DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edge_data(int edge_id)
    {
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        auto data = edges_data.find(edge_id);
        if (data == edges_data.end()) throw std::invalid_argument("No data is associated with edge" + std::to_string(edge_id) + ".");
        
        return data->second;
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::tuple<int, int> DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edge_nodes(int edge_id) const
    {
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        return it->second;
    }

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::mark_as_erased(int edge_id){
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        erased_edges.insert(edge_id);
    }

    template <typename VertexDataType, typename EdgeDataType>
    void DirectedLabeledGraph<VertexDataType, EdgeDataType>::unmark_as_erased(int edge_id){
        auto it = id_to_edge_mapping.find(edge_id);
        if (it == id_to_edge_mapping.end()) throw std::invalid_argument("Given edge_id " + std::to_string(edge_id) + " does not identify an edge.");
        erased_edges.erase(edge_id);
    }

    /* End of edge operations */

    template <typename VertexDataType, typename EdgeDataType>
    std::string DirectedLabeledGraph<VertexDataType, EdgeDataType>::convert_to_dot() const
    {
        std::ostringstream dotStream;
        std::string label;

        dotStream << "digraph G {" << std::endl;

        dotStream << "\t// Node definitions " << std::endl;
        for (const auto &node : outgoing_edges)
        {
            dotStream << "\t" << node.first << " [label=\"" << node.first << "\"];" << std::endl;
        }

        dotStream << "\n";
        dotStream << "\t// Edge definitions " << std::endl;
        for (auto &node : outgoing_edges)
        {
            int src = node.first;
            const std::list<OutgoingEdge> &edges_from_src = node.second;
            for (const auto &edge : edges_from_src)
            {
                // try {
                //     label = edges_data.at(edge.id).to_string();
                // } catch (...){
                //     label = "";
                // }
                dotStream << "\t" << src << " -> " << edge.to <<   ";" << std::endl; // " [label=\"" << edge.id << " : " << label << "\"]" << ";" << std::endl;
            }
        }
        dotStream << "}\n"
                  << std::endl;
        return dotStream.str();
    }

    template <typename VertexDataType, typename EdgeDataType>
    void __scc_search(const DirectedLabeledGraph<VertexDataType, EdgeDataType> *g, int vertex,
                      std::unordered_map<int, std::tuple<int, int, int>> &vertices,
                      int &vertex_counter, int &scc_counter)
    {
        std::stack<int> scc_stack;
        std::stack<std::tuple<int, int>> dft_stack; // (src, successor)
        int src, current_vertex, low_link, vertex_id, scc_id;
        int successor_low_link, successor_vertex_id, successor_scc_id;

        dft_stack.push({-1, vertex});
        while (!dft_stack.empty())
        {
            std::tie(src, current_vertex) = dft_stack.top();
            std::tie(low_link, vertex_id, scc_id) = vertices[current_vertex];

            if (scc_id == -1)
            {
                vertex_counter++;
                vertices[current_vertex] = {vertex_counter, vertex_counter, 0};
                std::tie(low_link, vertex_id, scc_id) = vertices[current_vertex];
                scc_stack.push(current_vertex);

                // for each successor
                for (int successor : g->get_successors(current_vertex))
                {
                    std::tie(successor_low_link, successor_vertex_id, successor_scc_id) = vertices[successor];
                    if (successor_scc_id == -1)
                    {
                        dft_stack.push({current_vertex, successor});
                    }
                    else if (successor_scc_id == 0)
                    {
                        low_link = std::min(low_link, successor_low_link);
                        std::get<0>(vertices[current_vertex]) = low_link;
                    }
                    // else :: already assigned SCC - pass
                }
            }
            else if (scc_id == 0)
            {
                dft_stack.pop();

                if (src != -1)
                {
                    std::tie(successor_low_link, successor_vertex_id, successor_scc_id) = vertices[src];
                    successor_low_link = std::min(low_link, successor_low_link);
                    std::get<0>(vertices[src]) = successor_low_link;
                }

                if (vertex_id == low_link)
                {
                    scc_counter++;
                    bool root_found = false;
                    while (!root_found)
                    {
                        current_vertex = scc_stack.top();
                        std::get<2>(vertices[current_vertex]) = scc_counter;
                        if (std::get<1>(vertices[current_vertex]) == vertex_id)
                        {
                            root_found = true;
                        }
                        scc_stack.pop();
                    }
                }
            }
            else
            {
                dft_stack.pop();
            }
        }
    }

    template <typename VertexDataType, typename EdgeDataType>
    std::unordered_map<int, int> DirectedLabeledGraph<VertexDataType, EdgeDataType>::strongly_connected_components()
    {
        using LowLinkValue = int;
        using VertexId = int;
        using SCCId = int;
        VertexId vertex_counter = 0;
        SCCId scc_counter = 0;
        std::unordered_map<int, std::tuple<LowLinkValue, VertexId, SCCId>> vertices;

        for (int vertex : get_nodes())
        {
            vertices[vertex] = {-1, -1, -1};
        }

        // DFT
        // for node in nodes:
        // if scc = -1 -> traverse
        int low_link, vertex_id, scc_id;
        for (int vertex : get_nodes())
        {
            std::tie(low_link, vertex_id, scc_id) = vertices[vertex];
            if (scc_id == -1)
            {
                __scc_search<VertexDataType, EdgeDataType>(this, vertex, vertices, vertex_counter, scc_counter);
            }
        }
        std::unordered_map<int, int> sccs;
        for (const auto &it : vertices)
        {
            sccs[it.first] = std::get<2>(it.second);
        }

        return sccs;
    }

    /* End of DirectedLabeledGraph */
}
template class graphs::DirectedLabeledGraph<std::string, LTSTransitionLabel>;
template class graphs::DirectedLabeledGraph<std::string, DCPTransitionLabel>;
template class graphs::DirectedLabeledGraph<std::pair<int, std::string>, std::string>;
template class graphs::DirectedLabeledGraph<std::shared_ptr<Expression>, std::pair<int, std::shared_ptr<Expression>>>;