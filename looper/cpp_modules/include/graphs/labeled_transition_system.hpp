#ifndef LTS_H
#define LTS_H

#include "graphs/directed_labeled_graph.hpp"
#include <vector>

namespace graphs {

   // LTS = (L, T, l_s, l_e)
   template <typename NodeDataType, typename EdgeDataType>
   // TODO: traits for NodeDataType and EdgeDataType to be convertible to string for convert_to_dot function
   class LabeledTransitionSystem : public DirectedLabeledGraph<NodeDataType,EdgeDataType>
   {
    private:
        int start_location;
        int end_location;
        std::vector<std::pair<std::string, std::string>> parameters;
        int n_params = 0;
        std::string fname;
        std::string ret;
        // TODO: required attributes
        // set of norms - update on each add_transition
        
    public:
      void add_parameter(std::string name, std::string type);
      void add_parameters(std::vector<std::pair<std::string, std::string>>);
      void add_ret(std::string name);
      void add_name(std::string name);

      int add_start_location();
      int add_start_location(NodeDataType data);

      int add_end_location();
      int add_end_location(NodeDataType data);

      int add_location();
      int add_location(NodeDataType data);

      int add_transition(int src, int dst, EdgeDataType label);
      int add_transition(int src, int dst);

      
      // Getters - ?
      int startLocation() const {return start_location;}
      int endLocation() const {return end_location;}

      std::string convert_to_dot() override; // override
   };

} // namespace graphs

#endif