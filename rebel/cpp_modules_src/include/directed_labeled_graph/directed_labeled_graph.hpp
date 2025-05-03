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

#ifndef DLG_H
#define DLG_H

#include <unordered_map>
#include <list>
#include <string>
#include <functional>
#include <vector>
#include <tuple>
#include <exception>
#include <optional>
#include <unordered_set>

namespace directed_labeled_graph
{
   /**
    * @brief Class representing directed labeled graph.
    *
    * Graph structure is represented using a pair of maps with adjency lists,
    * the first one for outgoing edges, the later one for ingoing edges.
    * Each added edge, vertex is automatically assigned an ID of vertex/edge counter
    * which is incremented with every added edge/node.
    * NOTE: Uniquenes of the ID is not checked and the implementation relies
    * on the counter not overflowing. Free id smaller than counter are not reused.
    * TODO: more robust solution
    *
    * Both vertex and edge data are stored separately from adjency lists in unordered maps
    * and the types are specified via template.
    * A vertex/node is not necessarily assigned data.
    * In such case when the data are accessed an exception is throw.
    *
    * Pairs source vertex, destination vertex of an edge are stored in unordered map id_to_edge_mapping.
    *
    *
    * @tparam VertexDataType Type of data stored per vertex
    * @tparam LabelDataType  Type of the vertex data
    */
   template <typename VertexDataType, typename EdgeDataType>
   class DirectedLabeledGraph
   {
   protected:
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
         int id;   ///> unique edge id - id of outgoing and corresponding ingoing edge is the same.

         IngoingEdge(int from, int edge_id)
             : from(from), id(edge_id) {}
      };

      ///> Adjacency list for outgoing edges
      std::unordered_map<int, std::list<OutgoingEdge>> outgoing_edges;
      ///> Adjacency list for ingoing edges
      std::unordered_map<int, std::list<IngoingEdge>> ingoing_edges;
      ///> Mapping from edge id to (src, dst) pair
      std::unordered_map<int, std::tuple<int, int>> id_to_edge_mapping;
      ///> Set of temporarily erased edges
      std::unordered_set<int> erased_edges;
      ///> Verteices data
      std::unordered_map<int, VertexDataType> vertices_data;
      ///> Edge data
      std::unordered_map<int, EdgeDataType> edges_data;

      ///> Vertex id counter
      int vertex_id_counter = 0;
      ///> Edge id counter
      int edge_id_counter = 0;

   public:
      /** @defgroup vertex_methods
       * Vertex methods
       */

      /* Beginning of vertex operations */
      /**
       * @brief Add a new vertex with data to the graph.
       * @ingroup vertex_methods
       *
       * The vertex id is determined automatically using the vertex counter.
       *
       * @param data vertex data
       * @return int an id assigned to the created vertex
       */
      int add_vertex(VertexDataType data);

      /**
       * @brief Add a new vertex with data to the graph.
       * @ingroup vertex_methods
       *
       * The vertex id is determined automatically using the vertex counter.
       *
       * @return int an id assigned to the created vertex
       */
      int add_vertex();

      /**
       * @brief Delete a vertex with given id from the graph.
       * @ingroup vertex_methods
       *
       * All the ingoing and outgoing edges of the vertex are removed.
       *
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id id of the vertex to be deleted
       */
      void delete_vertex(int vertex_id);

      /**
       * @brief Assign data to a vertex with given id.
       * @ingroup vertex_methods
       *
       * If data is already assigned to the vertex, original data is overwritten.
       *
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id id of a vertex
       * @param data data to be assigned
       */
      void set_vertex_data(int vertex_id, VertexDataType data);

      /**
       * @brief Clear data of a vertex with given id.
       * @ingroup vertex_methods
       *
       * If data is not assigned to the vertex, method has no effect.
       *
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id if of a vertex
       */
      void clear_vertex_data(int vertex_id);

      /**
       * @brief Get a pointer to data of a vertex with given id.
       * @ingroup vertex_methods
       *
       *
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @throws std::invalid_argument if the vertex is not assigned data
       * @param vertex_id id of a vertex
       * @return VertexDataType a pointer to the data assigned to the vertex
       */
      VertexDataType &get_vertex_data(int vertex_id);

      /**
       * @brief Get vector fo edges between vertices src and dst.
       * @ingroup vertex_methods
       *
       * @throws std::invalid_argument if the src or dst does not identify a vertex
       * @param src source vertex
       * @param dst destination vertex
       * @return std::vector vector of edge ids
       */
      std::vector<int> get_edges_between(int src, int dst) const;

      /**
       * @brief Get vector of outgoing edges for vertex src.
       * @ingroup vertex_methods
       *
       * @throws std::invalid_argument if the src does not identify a vertex
       * @param src source vertex
       * @return std::vector<std::tuple<int, int>> vector of pairs (edge id, source vertex)
       */
      std::vector<std::tuple<int, int>> get_outgoing_edges(int src) const;

      /**
       * @brief Get vector of ingoing edges for vertex src.
       * @ingroup vertex_methods
       *
       * @throws std::invalid_argument if the dst does not identify a vertex
       * @param src destination vertex
       * @return std::vector<std::tuple<int, int>> vector of pairs (edge id, destination vertex)
       */
      std::vector<std::tuple<int, int>> get_ingoing_edges(int dst) const;

      /**
       * @brief Get vector of successor vertices of vertex src.
       * @ingroup vertex_methods
       *
       * @throws std::invalid_argument if the src does not identify a vertex
       * @param src source vertex
       * @return std::vector<int> vector of ids of successor vertices
       */
      std::vector<int> get_successors(int src) const;

      /**
       * @brief Get vector of predecessor vertices of vertex src.
       * @ingroup vertex_methods
       *
       * @throws std::invalid_argument if the dst does not identify a vertex
       * @param dst destination vertex
       * @return std::vector<int> vector of ids of predecessor vertices
       */
      std::vector<int> get_predecessors(int dst) const;
      /* End of node operations */

      
      /** @defgroup edge_methods
       * Edge methods
       */
      /**
       * @brief Add a new edge (src, dst) with given label to the graph.
       * @ingroup edge_methods
       *
       * If an edge identified with src, dst already exists, another one
       * is created with own id.
       *
       * @throws std::invalid_argument if src or dst does not identify a vertex
       * @param src source vertex
       * @param dst target vertex
       * @param label edge label
       * @return int id of the created edge
       */
      int add_edge(int src, int dst, EdgeDataType label);

      /**
       * @brief Add a new edge (src, dst) to the graph
       * @ingroup edge_methods
       *
       * If an edge identified with src, dst already exists, another one
       * is created with own id.
       *
       * @throws std::invalid_argument if src or dst does not identify a vertex
       * @param src source vertex
       * @param dst target vertex
       * @param label edge label
       * @return int id of the created edge
       */
      int add_edge(int src, int dst);


      void mark_as_erased(int edge_id);
      void unmark_as_erased(int edge_id);

      /**
       * @brief Delete an edge with given id.
       * @ingroup edge_methods
       *
       * @throws std::invalid_argument if the edge_id does not identify an edge
       * @param src source node
       * @param dst target node
       */
      void delete_edge(int edge_id);

      /**
       * @brief Assign data to an edge with given id.
       * @ingroup edge_methods
       *
       * If data is already assigned to the edge, original data is overwritten.
       *
       * @throws std::invalid_argument if the edge_id does not identify an edge
       * @param edge_id id of an edge
       * @param data data to be assigned
       */
      void set_edge_data(int edge_id, EdgeDataType data);

      /**
       * @brief Clear data of an edge with given id.
       * @ingroup edge_methods
       *
       * If data is not assigned to the edge, method has no effect.
       *
       * @throws std::invalid_argument if the edge_id does not identify an edge
       * @param edge_id id of an edge
       */
      void clear_edge_data(int edge_id);

      /**
       * @brief Get vector of pairs (src, dst) vertices of an edge with given edge_id
       * @ingroup edge_methods
       *
       * @throws std::invalid_argument if the edge_id does not identify an edge
       * @param edge_id id of an edge
       * @return std::tuple<int, int> vector of pairs (src, dst) vertices
       */
      std::tuple<int, int> get_edge_nodes(int edge_id) const;

      /**
       * @brief Get a pointer to data of an edge with given id.
       * @ingroup edge_methods
       *
       * @throws std::invalid_argument if the edge_id does not identify an edge
       * @throws std::invalid_argument if the edge is not assigned data
       * @param edge_id id of an edge
       * @return EdgeDataType a pointer to the data assigned to the edge
       */
      EdgeDataType &get_edge_data(int edge_id);

      /* End of edge operations */

      /**
       * @brief Calculate strongly connected components of a graph.
       *
       * @return std::unordered_map<int, int> mapping of vertices to ids of strongly connected components
       */
      std::unordered_map<int, int> strongly_connected_components();

      /**
       * @brief Convert graph into .dot format and print it to stdout
       *
       */
      virtual std::string convert_to_dot() const;

      // Getters
      const std::unordered_map<int, std::list<OutgoingEdge>> &get_outgoing_edges() const { return outgoing_edges; };
      const std::unordered_map<int, std::list<IngoingEdge>> &get_ingoing_edges() const { return ingoing_edges; };
      const std::unordered_map<int, std::tuple<int, int>> &get_id_to_edge_mapping() const { return id_to_edge_mapping; };
      const std::unordered_map<int, VertexDataType> &get_all_node_data() const { return vertices_data; };
      const std::unordered_map<int, EdgeDataType> &get_all_edge_data() const { return edges_data; };
      std::vector<int> get_nodes() const
      {
         std::vector<int> nodes;
         for (const auto &n : outgoing_edges)
            nodes.push_back(n.first);
         return nodes;
      }
      std::vector<int> get_edges() const
      {
         std::vector<int> edges;
         for (const auto &e : id_to_edge_mapping){
            if (erased_edges.find(e.first) != erased_edges.end()) continue;
            edges.push_back(e.first);
         }
         return edges;
      }
      int get_node_id_counter() const { return vertex_id_counter; };
      int get_edge_id_counter() const { return edge_id_counter; };
   };

} // namespace graphs


#endif