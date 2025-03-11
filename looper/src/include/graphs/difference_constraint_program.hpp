#ifndef DCP_H
#define DCP_H

#include "graphs/directed_labeled_graph.hpp"
#include "graphs/dcp_labels.hpp"
#include <vector>
#include <unordered_set>

namespace graphs {  

   class LabeledTransitionSystem;

   class DifferenceConstraintProgram : public DirectedLabeledGraph<std::string, DCPTransitionLabel>
   {
    private:
        int start_location;
        int end_location;
        std::unordered_set<int> back_edges;    
        std::unordered_set<int> loop_heads;    
    public:
      
      // Getters
      int get_start_location() const {return start_location;}
      int get_end_location() const {return end_location;}
      const std::unordered_set<int> get_back_edges() const {return back_edges;}
      const std::unordered_set<int> get_loop_heads() const {return loop_heads;}
      bool is_loop_head(int vertex_id) const;
      bool is_back_edge(int edge_id) const;

      // LTS to DCP Mapper
      friend void lts_to_dcp_mapper(LabeledTransitionSystem& lts, DifferenceConstraintProgram& dcp);

      std::string convert_to_dot() const override;
   };

} // namespace graphs 

#endif