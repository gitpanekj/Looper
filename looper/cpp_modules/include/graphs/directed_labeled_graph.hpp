/**
 * @file directed_labeled_graph.hpp
 * @author Jan Pánek xpanek11@stud.fit.vutbr.cz
 * @brief Header file for directed_labeled_graph.cpp
 * @version 0.1
 * @date 2024-10-08
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
 TODOs
 * TODO: better solution for id generation
 * TODO: add support for id -> (src, dst) lookup
*/

#ifndef DLG_H
#define DLG_H

#include <unordered_map>
#include <list>
#include <string>
#include <functional>
#include <vector>
#include <tuple>

namespace graphs
{
   /**
    * @brief Class for directed labeled graph
    *
    * @tparam NodeDataType
    * @tparam LabelDataType
    */
   template <typename NodeDataType, typename EdgeDataType>
   // TODO: traits for NodeDataType and EdgeDataType to be convertible to string for convert_to_dot function
   class DirectedLabeledGraph
   {
   private:
      ///> OutgoingEdge structure
      struct OutgoingEdge
      {
         int to; ///> edge destination
         int id; ///> unique edge id - id of outgoing and corresponding ingoing edge is the same.
         OutgoingEdge(int to, int edge_id)
             : to(to), id(edge_id) {}
      };

      ///> IngoingEdge structure
      struct IngoingEdge
      {
         int from; ///> edge destination
         int id;                      ///> unique edge id - id of outgoing and corresponding ingoing edge is the same.

         IngoingEdge(int from, int edge_id)
             : from(from), id(edge_id) {}
      };

      ///> Adjacency list for outgoing edges
      std::unordered_map<int, std::list<OutgoingEdge>> outgoing_edges;
      ///> Adjacency list for ingoing edges
      std::unordered_map<int, std::list<IngoingEdge>> ingoing_edges;
      ///> Mapping from edge id to (src, dst) pair
      std::unordered_map<int, std::tuple<int, int>> id_to_edge_mapping;
      ///> Node data
      std::unordered_map<int, NodeDataType> nodes;
      ///> Edge data
      std::unordered_map<int, EdgeDataType> edges;

      // TODO: better solution for id generation
      ///> Node id counter
      int node_id_counter = 0;
      ///> Edge id counter
      int edge_id_counter = 0;

   public:
      /* Beginning of node operations */
      /**
       * @brief Add a new node with given id to the graph including data
       *
       */
      int add_node(NodeDataType data);

      /**
       * @brief Add a new node with given id to the graph
       *
       */
      int add_node();

      /**
       * @brief Delete a node with given id from the graph
       *
       * @param id deleted node id
       */
      void delete_node(int node_id);

      /**
       * @brief Set the node data
       *
       * @param node_id
       * @param node
       */
      void set_node_data(int node_id, NodeDataType node);

      /**
       * @brief Reset the node data
       *
       * @param node_id
       * @param node
       */
      void reset_node_data(int node_id);

      /**
       * @brief Update the node data using update function.
       *
       * @param node_id
       */
      void update_node(int node_id, std::function<void(NodeDataType &node)> update_function);

      /**
       * @brief Get data of the node with given node_id
       *
       * @param node_id
       * @return NodeDataType
       */
      const NodeDataType &get_node_data(int node_id);

      /**
       * @brief Get the edges between src and dst nodes
       *
       * @param src
       * @param dst
       * @return std::vector
       */
      std::vector<int> get_edges_between(int src, int dst);

      /* End of node operations */

      /* Beginning of node operations */
      /**
       * @brief Add a new edge (src, dst) with given label to the graph
       *
       * @param src source node
       * @param dst target node
       * @param label edge label
       */
      int add_edge(int src, int dst, EdgeDataType label);

      /**
       * @brief Add a new edge (src, dst) with given label to the graph
       *
       * @param src source node
       * @param dst target node
       * @param label edge label
       */
      int add_edge(int src, int dst);

      /**
       * @brief Delete the edge with given id
       *
       * @param src source node
       * @param dst target node
       */
      void delete_edge(int edge_id);

      /**
       * @brief Set the edge data
       *
       * @param edge_id
       * @param edge
       */
      void set_edge_data(int edge_id, EdgeDataType edge);

      /**
       * @brief Set the edge data
       *
       * @param edge_id
       * @param edge
       */
      void reset_edge_data(int edge_id);

      /**
       * @brief Update the edge data using update function.
       *
       * @param edge_id
       */
      void update_edge(int edge_id, std::function<void(EdgeDataType &edge)> update_function);

      /**
       * @brief Get the src and dst nodes of the edge with given edge_id
       *
       * @param edge_id
       * @return std::tuple<int, int>
       */
      std::tuple<int, int> get_edge_nodes(int edge_id);

      /**
       * @brief Get data of the edge with given edge_id
       *
       * @param edge_id
       * @return EdgeDataType
       */
      const EdgeDataType &get_edge_data(int edge_id);

      /* End of node operations */

      /**
       * @brief Convert graph into .dot format and print it to stdout
       *
       */
      virtual std::string convert_to_dot();

      // TODO: compute Strongly Connected Components

      // Getters
      const std::unordered_map<int, std::list<OutgoingEdge>> &getOutgoingEdges() const;
      const std::unordered_map<int, std::list<IngoingEdge>> &getIngoingEdges() const;
      const std::unordered_map<int, std::tuple<int, int>> &getIdToEdgeMapping() const;
      const std::unordered_map<int, NodeDataType> &getNodeData() const;
      const std::unordered_map<int, EdgeDataType> &getEdgeData() const;
      int getNodeIdCounter() const;
      int getEdgeIdCounter() const;
   };

} // namespace graphs

#endif