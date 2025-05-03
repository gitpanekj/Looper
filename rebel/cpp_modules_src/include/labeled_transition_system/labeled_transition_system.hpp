#ifndef LTS_H
#define LTS_H

#include "directed_labeled_graph/directed_labeled_graph.hpp"
#include "difference_constraint_program/difference_constraint_program.hpp"
#include "lts_labels.hpp"
#include <vector>
#include <unordered_set>


namespace labeled_transition_system {
   /**
    * @brief Implementation of Labeled Transition System (LTS) abstracting program execution.
    * 
    * Formally labeled transition system is a graph structure represented by four tuple (L, T, l_s, l_e)
    * where L is a finite set of program locations, T \subseteq L \times L is a finite set of transitions
    * between program locations labeled with transition relations which determine how a program state changes
    * as a result of the transition execution. l_s and l_e are in turn start and exit location of the program.
    * 
    * An LTS represents a function with a signature determined by it's parameters (parameters) name (fname)
    * and type of return value (ret).
    * 
    * For keeping track of loop head vertex ids along with related back edge ids, two ordered sets are used.
    * 
    */
   class LabeledTransitionSystem : public directed_labeled_graph::DirectedLabeledGraph<std::string, LTSTransitionLabel>
   {
    private:
        int start_location;
        int end_location;
        std::unordered_set<int> back_edges;
        std::unordered_set<int> loop_heads;


        std::vector<std::pair<std::string, std::string>> parameters;
        int n_params = 0;
        std::string fname;
        std::string ret;
        
    public:
      /* methods related to  procedure metadata */
      /**
       * @defgroup procedure_metadata
       * Methods related to metadata an LTS represents
       */

      /**
       * @brief Add a parameter along with it's type to the procedure parameter list.
       * @ingroup procedure_metadata
       * 
       * @param name parameter name
       * @param type parameter type
       */
      void add_parameter(std::string name, std::string type);

      /**
       * @brief Add a parameters along with it's types to the procedure parameter list.
       * @ingroup procedure_metadata
       * 
       * @param param_list list of pairs (parameter, type)
       */
      void add_parameters(std::vector<std::pair<std::string, std::string>> param_list);

      /**
       * @brief Specify a return value type of the procedure.
       * @ingroup procedure_metadata
       * 
       * Original return type is overwritten if set.
       * 
       * @param type return value type
       */
      void set_return_type(std::string type);

      /**
       * @brief Specify a name of the procedure.
       * @ingroup procedure_metadata
       * 
       * Original name is overwritten if set.
       * 
       * @param name procedure name
       */
      void set_function_name(std::string name);


      /**
       * @defgroup lts_structure
       * Methods related LTS structure
       */

      /**
       * @brief Add a new vertex and mark it as start location.
       * @ingroup lts_structure
       * 
       * Original start location is overwritten if set.
       * 
       * @return int id of the created vertex
       */
      int add_start_location();

      /**
       * @brief Add a new vertex with data and mark it as start location.
       * @ingroup lts_structure
       * 
       * Original start location is overwritten if set.
       * 
       * @param data vertex data
       * @return int id of the created vertex
       */
      int add_start_location(std::string data);

      /**
       * @brief Add a new vertex and mark it as end location.
       * @ingroup lts_structure
       * 
       * Original start location is overwritten if set.
       * 
       * @return int id of the created vertex
       */      
      int add_end_location();

      /**
       * @brief Add a new vertex with data and mark it as end location.
       * @ingroup lts_structure
       * 
       * Original start location is overwritten if set.
       * 
       * @param data vertex data
       * @return int id of the created vertex
       */
      int add_end_location(std::string data);


      /**
       * @brief Mark vertex with given vertex id as loop head.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id
       */
      void mark_loop_head(int vertex_id);

      /**
       * @brief Unmark vertex with given vertex id as loop head.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id
       */
      void unmark_loop_head(int vertex_id);

      /**
       * @brief Mark edge with given edge id as back edge.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify an edge
       * @param vertex_id
       */
      void mark_back_edge(int edge_id);

      /**
       * @brief Unmark edge with given edge id as back edge.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify an edge
       * @param vertex_id
       */
      void unmark_back_edge(int edge_id);

      /**
       * @brief Check whether a vertex with vertex_id is a loop head.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify a vertex
       * @param vertex_id
       */
      bool is_loop_head(int vertex_id) const;

      /**
       * @brief Check whether an edge with edge_id is a back edge.
       * @ingroup lts_structure
       * 
       * @throws std::invalid_argument if the vertex_id does not identify an edge
       * @param vertex_id
       */
      bool is_back_edge(int edge_id) const;
      

      // Getters
      int get_start_location() const {return start_location;}
      int get_end_location() const {return end_location;}
      const std::unordered_set<int> get_back_edges() const {return back_edges;}
      const std::unordered_set<int> get_loop_heads() const {return loop_heads;}
      std::vector<std::pair<std::string, std::string>> get_parameters() const { return parameters;}
      std::string get_function_name() const { return fname; }
      std::string get_return_type() const { return ret; }   

      std::string convert_to_dot() const;
   };

} // namespace graphs

#endif