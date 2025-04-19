#include "expression/expression_ast.hpp"
#include <vector>
#include <memory>
#include <sstream>

// TODO: advanced comparison of max terms
// now only constants and axact match of terms is considered

ASTMaxNode::ASTMaxNode(std::vector<std::shared_ptr<ASTNodeBase>> terms) : ASTNodeBase(ASTNodeType::Max)
{
    std::shared_ptr<ASTNodeBase> max_const;
    bool const_present = false;
    std::unordered_set<std::string> term_names;

    for (int i=0;i<terms.size();i++)
    {
        auto term = terms[i];
        if (term->get_variable_terms().size() == 0) // constant
        {
            if (const_present)
            {
                if (term->get_constant_term() > max_const->get_constant_term()) max_const = term;
            }
            else
            {
                const_present = true;
                max_const = term;
            }
        }
        else if (term_names.find(term->to_string()) == term_names.end()) // no exact match of a variable expression
        {
            this->terms.push_back(term);
            term_names.insert(term->to_string());
        }
    }

    if (const_present)
    {
        this->terms.push_back(max_const);
    }

    upadte_expression_string();
    upadte_hash_string();
}


std::shared_ptr<ASTNodeBase>ASTMaxNode::copy(){
    std::vector<std::shared_ptr<ASTNodeBase>> terms_copy;
    for (const auto &term : terms)
    {
        terms_copy.push_back(term->copy());
    }

    return std::make_shared<ASTMaxNode>(terms_copy);
}


std::shared_ptr<ASTNodeBase> ASTMaxNode::substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr)
{
    for (int i=0;i<terms.size();i++)
    {
        terms[i] = terms[i]->substitute(_name, expr);
    }

    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
}

std::shared_ptr<ASTNodeBase> ASTMaxNode::expand()
{
    for (int i=0;i<terms.size();i++)
    {
        terms[i] = terms[i]->expand();
    }

    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
}


std::vector<std::shared_ptr<ASTNodeBase>> ASTMaxNode::get_variable_terms() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms;

    std::unordered_set<std::string> names;

    for (const auto& term : this->terms)
    {
        term->get_variable_names(names);
        if (names.size() == 0) continue; // No variable present in the expression
        names.clear();
        terms.push_back(term);
    }

    return terms; 

}
// Operands
std::vector<std::shared_ptr<ASTNodeBase>> ASTMaxNode::get_operands() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms;

    for (const auto &term : this->terms)
    {
        terms.push_back(term);
    }

    return terms;
}

// Repr
std::string ASTMaxNode::_to_string() const
{
    std::stringstream ss;
    ss << "max(";

    for (auto it = terms.begin(); it != terms.end(); it++)
    {
        ss << (*it)->_to_string() << ((it == (--terms.end())) ? "" : ", ");
    }

    ss << ")";

    return ss.str();
}


std::string ASTMaxNode::_to_hash() const
{
    std::stringstream ss;
    ss << "max(";

    for (auto it = terms.begin(); it != terms.end(); it++)
    {
        ss << (*it)->_to_hash() << ((it == (--terms.end())) ? "" : ", ");
    }

    ss << ")";

    return ss.str();
}