#include "graphs/labeled_transition_system.hpp"

namespace graphs {
    /* LTS */
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
        end_location = this->add_node();
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


    template <typename NodeDataType, typename EdgeDataType>
    std::string LabeledTransitionSystem<NodeDataType, EdgeDataType>::convert_to_dot()
    {
        return DirectedLabeledGraph<NodeDataType, EdgeDataType>::convert_to_dot();
    }
} // namespaces graphs

// Templates
template class graphs::LabeledTransitionSystem<std::string, std::string>;