#include "expression/expression_ast.hpp"
#include <vector>
#include <memory>
#include <sstream>

ASTMinNode::ASTMinNode(std::vector<std::shared_ptr<ASTNodeBase>> terms) : ASTNodeBase(ASTNodeType::Min)
{
    for (const auto& term : terms)
    {
        this->terms.push_back(term);
    }
}


std::shared_ptr<ASTNodeBase>ASTMinNode::copy(){
    std::vector<std::shared_ptr<ASTNodeBase>> terms_copy;
    for (const auto &term : terms)
    {
        terms_copy.push_back(term->copy());
    }

    return std::make_shared<ASTMinNode>(terms_copy);
}


std::shared_ptr<ASTNodeBase> ASTMinNode::substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr)
{
    for (int i=0;i<terms.size();i++)
    {
        terms[i] = terms[i]->substitute(_name, expr);
    }

    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
}

std::shared_ptr<ASTNodeBase> ASTMinNode::expand()
{
    for (int i=0;i<terms.size();i++)
    {
        terms[i] = terms[i]->expand();
    }

    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
}


std::vector<std::shared_ptr<ASTNodeBase>> ASTMinNode::get_variable_terms() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms;

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::unordered_set<std::string> names;

    for (const auto& term : this->terms)
    {
        term->get_variable_names(names);
        if (names.size() == 0) continue; // No variable present in the expression
        names.clear();
        terms.push_back(value);
    }

    return terms; 

}
// Operands
std::vector<std::shared_ptr<ASTNodeBase>> ASTMinNode::get_operands() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms;

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    for (const auto &term : this->terms)
    {
        terms.push_back(value);
    }

    return terms;
}

// Repr
std::string ASTMinNode::_to_string() const
{
    std::stringstream ss;
    ss << "min(";

    for (auto it = terms.begin(); it != terms.end(); it++)
    {
        ss << (*it)->_to_string() << ((it == (--terms.end())) ? "" : ", ");
    }

    ss << ")";

    return ss.str();
}


std::string ASTMinNode::_to_hash() const
{
    std::stringstream ss;
    ss << "min(";

    for (auto it = terms.begin(); it != terms.end(); it++)
    {
        ss << (*it)->_to_hash() << ((it == (--terms.end())) ? "" : ", ");
    }

    ss << ")";

    return ss.str();
}