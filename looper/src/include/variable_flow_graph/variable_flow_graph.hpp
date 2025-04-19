#ifndef VFG_H
#define VFG_H

#include "directed_labeled_graph/directed_labeled_graph.hpp"
#include <unordered_map>
#include <utility>


namespace variable_flow_graph
{
    using Location = int;
    using VarName = std::string;
    using VertexId = int;
    class VariableFlowGraph : public directed_labeled_graph::DirectedLabeledGraph<std::pair<Location, VarName>, std::string>
    {
    private:
        struct pair_hash
        {
            std::size_t operator()(const std::pair<Location, VarName> &p) const
            {
                std::size_t h1 = std::hash<Location>{}(p.first);
                std::size_t h2 = std::hash<VarName>{}(p.second);
                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<std::pair<Location, VarName>, VertexId, pair_hash> vertex_label_to_vertex_id_mapping;

    public:
        int add_variable_vertex(int location, std::string variable);
        void delete_variable_vertex(int location, std::string variable);
        int find_variable_vertex(int location, std::string variable);
        std::string convert_to_dot() const;

        int add_vertex(std::pair<Location, VarName> data) = delete;
        void delete_vertex(int vertex_id) = delete;

        // Deleted methods - a vertex must always have a label (location, varname)
        int add_vertex() = delete;
        void set_vertex_data(int vertex_id, std::pair<Location, VarName> data) = delete;
        void clear_vertex_data(int vertex_id) = delete;
    };
}
#endif
