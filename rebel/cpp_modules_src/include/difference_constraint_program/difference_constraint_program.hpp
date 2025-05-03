#ifndef DCP_H
#define DCP_H

#include "directed_labeled_graph/directed_labeled_graph.hpp"
#include "labeled_transition_system/labeled_transition_system.hpp"
#include "dcp_labels.hpp"
#include <vector>
#include <unordered_set>

namespace labeled_transition_system {
   class LabeledTransitionSystem;
}


namespace difference_constraint_program { 
   class DifferenceConstraintProgram : public directed_labeled_graph::DirectedLabeledGraph<std::string, DCPTransitionLabel>
   {
    private:
        int start_location;
        int end_location;
        std::unordered_set<int> back_edges;    
        std::unordered_set<int> loop_heads;

        std::vector<std::pair<std::string, std::string>> parameters;
    public:
      
      // Getters
      int get_start_location() const {return start_location;}
      int get_end_location() const {return end_location;}
      const std::unordered_set<int> get_back_edges() const {return back_edges;}
      const std::unordered_set<int> get_loop_heads() const {return loop_heads;}
      std::vector<std::pair<std::string, std::string>> get_parameters() const { return parameters;}
      bool is_loop_head(int vertex_id) const;
      bool is_back_edge(int edge_id) const;

      // LTS to DCP Mapper
      friend void lts_to_dcp_mapper(labeled_transition_system::LabeledTransitionSystem& lts, DifferenceConstraintProgram& dcp);

      std::string convert_to_dot() const override;
   };

} // namespace graphs 

#endif