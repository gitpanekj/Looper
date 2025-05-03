#ifndef RCG_H
#define RCG_H

#include "directed_labeled_graph/directed_labeled_graph.hpp"
#include "expression/expression.hpp"
#include <unordered_map>
#include <utility>
#include <memory>


namespace reset_chain_graph
{
    using Atom = std::shared_ptr<Expression>;
    using Const = std::shared_ptr<Expression>;
    using EdgeId = int;
    using Reset = std::pair<EdgeId, Const>;
    class ResetChainGraph : public directed_labeled_graph::DirectedLabeledGraph<Atom, Reset>
    {
    private:
        std::unordered_map<std::string, int> vertex_label_to_vertex_id_mapping;
    public:
        int add_atom(Atom atom);
        int find_atom_vertex(std::string atom);
        std::string convert_to_dot() const;
    };
}
#endif
