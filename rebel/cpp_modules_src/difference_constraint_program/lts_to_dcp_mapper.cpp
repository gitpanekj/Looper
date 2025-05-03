#include "difference_constraint_program/difference_constraint_program.hpp"
#include "difference_constraint_program/lts_to_dcp_mapper.hpp"
#include "labeled_transition_system/labeled_transition_system.hpp"


namespace difference_constraint_program
{
    /**
     * @brief Create dcp structure without edge labels for given lts.
     */
    void lts_to_dcp_mapper(labeled_transition_system::LabeledTransitionSystem& lts, DifferenceConstraintProgram& dcp)
    {
        // LTS related mappings
        dcp.start_location = lts.get_start_location();
        dcp.end_location = lts.get_end_location();
        dcp.back_edges = lts.get_back_edges();
        dcp.loop_heads = lts.get_loop_heads();
        dcp.parameters = lts.get_parameters();

        // DeepCopy

        // Copy outgoing edges
        for (const auto& [key, edge_list] : lts.get_outgoing_edges()) {
            dcp.outgoing_edges[key] = std::list<DifferenceConstraintProgram::OutgoingEdge>();
            for (const auto& elem : edge_list){
                dcp.outgoing_edges[key].push_back(DifferenceConstraintProgram::OutgoingEdge(elem.to, elem.id));
            }
        }

        // Copy ingoing edges
        for (const auto& [key, edge_list] : lts.get_ingoing_edges()) {
            dcp.ingoing_edges[key] = std::list<DifferenceConstraintProgram::IngoingEdge>();
            for (const auto& elem : edge_list){
                dcp.ingoing_edges[key].push_back(DifferenceConstraintProgram::IngoingEdge(elem.from, elem.id));
            }
        }

        // Copy edge if mapping
        for (const auto& [key, mapping] : lts.get_id_to_edge_mapping()) {
            dcp.id_to_edge_mapping[key] = mapping;
        }

        // Copy vertex labels
        for (const auto& [key, label] : lts.get_all_node_data()) {
            dcp.vertices_data[key] = label;
        }
    };
} // namespace  graphs
