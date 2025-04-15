/**
 * @file expr.cpp
 * @author Jan Pánek xpanek11@stud.fit.vutbr.cz
 * @brief Simple symbolic algebra multivariate expression representation.
 * @version 0.1
 * @date 2025-02-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "expression/expression_ast.hpp"

#include <string>
#include <memory>
#include <vector>
#include <sstream>

ASTMultiplicationNode::ASTMultiplicationNode(std::vector<std::shared_ptr<ASTNodeBase>> ops) : ASTNodeBase(ASTNodeType::Multiplication)
{
    constant_part = std::make_shared<ASTIntegerConstantNode>(1);
    for (const auto &op : ops)
    {
        process_operand(op);
    }

    upadte_expression_string();
    upadte_hash_string();
}

std::shared_ptr<ASTNodeBase> ASTMultiplicationNode::copy()
{
    std::vector<std::shared_ptr<ASTNodeBase>> operands;
    for (const auto &expr : variable_part)
    {
        operands.push_back(expr.second->copy());
    }
    operands.push_back(constant_part->copy());

    return std::make_shared<ASTMultiplicationNode>(operands);
}

std::shared_ptr<ASTNodeBase> ASTMultiplicationNode::substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr)
{

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::shared_ptr<ASTNodeBase> subst;
    std::vector<std::multimap<std::string, std::shared_ptr<ASTNodeBase>>::iterator> terms_to_delete;
    std::vector<std::shared_ptr<ASTNodeBase>> new_terms;

    for (auto it = variable_part.begin(); it != variable_part.end(); ++it)
    {

        std::tie(key, value) = *it;

        subst = value->substitute(_name, expr);
        if (subst->to_hash() != key) // hash changed
        {
            terms_to_delete.push_back(it);

            // Check constant - subexpression degraded to a constant
            if (subst->get_type() == ASTNodeType::IntegerConstant)
            {
                set_constant_term(get_constant_term() * subst->get_constant_term());
                continue;
            }

            // Adding a new term
            new_terms.push_back(subst);
        }
    }

    for (const auto &term : terms_to_delete)
    {
        variable_part.erase(term);
    }

    for (const auto &term : new_terms)
    {
        process_operand(term);
    }

    if (get_variable_terms().size() == 0)
    {
        return std::make_shared<ASTIntegerConstantNode>(get_constant_term());
    }

    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
};

std::shared_ptr<ASTNodeBase> ASTMultiplicationNode::expand()
{
    // Expanded form
    std::shared_ptr<ASTNodeBase> expanded_form = shared_from_this();

    // If any node is in form of addition
    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::shared_ptr<ASTNodeBase> expanded_operand;
    std::multimap<std::string, std::shared_ptr<ASTNodeBase>>::iterator term_to_delete = variable_part.end();
    std::vector<std::shared_ptr<ASTNodeBase>> new_terms;

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        std::tie(key, value) = *it;
        value = value->expand();
        if (value->get_type() == ASTNodeType::Addition) // Apply distributivity
        {
            term_to_delete = it;
            break;
        }
    }

    if (term_to_delete == variable_part.end()){
        return shared_from_this();
    }

    variable_part.erase(term_to_delete);
    for (auto term : value->get_operands())
    {
        expanded_operand = copy();
        expanded_operand = std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({expanded_operand, term}));
        if (expanded_operand->get_variable_terms().size() == 0)
        {
            expanded_operand = std::make_shared<ASTIntegerConstantNode>(expanded_operand->get_constant_term());
        }

        new_terms.push_back(expanded_operand);
    }

    expanded_form = std::make_shared<ASTAdditionNode>(new_terms);
    expanded_form = expanded_form->expand();


    return expanded_form;
}

std::string ASTMultiplicationNode::_to_string() const
{
    std::stringstream ss;

    int val = get_constant_term();
    if (val != 1)
        ss << std::to_string(val) << "*";

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        if ((*it).second->get_type() == ASTNodeType::Addition)
        {
            ss << "(" + (*it).second->_to_string() + ")";
        }
        else
        {
            ss << (*it).second->_to_string();
        }
        ss << ((it == (--variable_part.end())) ? "" : "*");
    }
    return ss.str();
}

std::string ASTMultiplicationNode::_to_hash() const
{
    std::stringstream ss;
    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        if ((*it).second->get_type() == ASTNodeType::Addition)
        {
            ss << "(" + (*it).second->_to_hash() + ")";
        }
        else
        {
            ss << (*it).second->_to_hash();
        }
        ss << ((it == (--variable_part.end())) ? "" : "*");
    }
    return ss.str();
}

void ASTMultiplicationNode::process_operand(std::shared_ptr<ASTNodeBase> op)
{
    ASTNodeType type = op->get_type();
    switch (type)
    {
    case ASTNodeType::AccessPath:
        variable_part.insert(std::pair{op->to_hash(), op});
        break;
    case ASTNodeType::Multiplication:
        for (const auto &mul_op : op->get_variable_terms())
        {
            variable_part.insert(std::pair{mul_op->to_hash(), mul_op});
        }
        set_constant_term(get_constant_term() * op->get_constant_term());
        break;
    case ASTNodeType::IntegerConstant:
        set_constant_term(get_constant_term() * op->get_constant_term());
        break;
    case ASTNodeType::Addition:
        if (op->get_operands().size() == 0) // 0
        {
            set_constant_term(0);
        }
        if (op->get_operands().size() == 1)
        {
            process_operand(op->get_operands()[0]);
        }
        else
        {
            variable_part.insert(std::pair{op->to_hash(), op});
        }
        break;
    case ASTNodeType::Max: // Represented as c * min, c * max
    case ASTNodeType::Min:
        variable_part.insert(std::pair{op->to_hash(), op});
        break;
    }
}

std::vector<std::shared_ptr<ASTNodeBase>> ASTMultiplicationNode::get_variable_terms() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> ops;

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    for (const auto &item : variable_part)
    {
        std::tie(key, value) = item;
        ops.push_back(value);
    }
    return ops;
}
// Operands
std::vector<std::shared_ptr<ASTNodeBase>> ASTMultiplicationNode::get_operands() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> ops;

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    for (const auto &item : variable_part)
    {
        std::tie(key, value) = item;
        ops.push_back(value);
    }
    
    ops.push_back(constant_part);
    
    return ops;
}
