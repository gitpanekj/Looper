/**
 * @file addition_ast_node.cpp
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

ASTAdditionNode::ASTAdditionNode(std::vector<std::shared_ptr<ASTNodeBase>> ops) : ASTNodeBase(ASTNodeType::Addition)
{
    constant_part = std::make_shared<ASTIntegerConstantNode>(0); // 0 is default constant term
    for (const auto &op : ops)
    {
        process_operand(op);
    }

    remove_zeros();
    upadte_expression_string();
    upadte_hash_string();
}

std::shared_ptr<ASTNodeBase> ASTAdditionNode::copy()
{
    std::vector<std::shared_ptr<ASTNodeBase>> operands;
    for (const auto &expr : variable_part)
    {
        operands.push_back(expr.second->copy());
    }
    operands.push_back(constant_part->copy());

    return std::make_shared<ASTAdditionNode>(operands);
}

std::shared_ptr<ASTNodeBase> ASTAdditionNode::substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr)
{
    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::shared_ptr<ASTNodeBase> subst;
    std::vector<std::multimap<std::string, std::shared_ptr<ASTNodeBase>>::iterator> terms_to_delete;
    std::vector<std::shared_ptr<ASTNodeBase>> new_terms;

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {

        std::tie(key, value) = *it;
        subst = value->substitute(_name, expr);

        if (subst->to_hash() != key) // hash changed
        {
            terms_to_delete.push_back(it);

            // Check constant - subexpression degraded to a constant
            if (subst->get_type() == ASTNodeType::IntegerConstant)
            {
                set_constant_term(get_constant_term() + subst->get_constant_term());
                continue;
            }

            // If such term is present, increase its multiplication factor
            if (auto it = variable_part.find(subst->to_hash()); it != variable_part.end())
            {
                std::tie(key, value) = *it;
                value->set_constant_term(value->get_constant_term() + subst->get_constant_term());
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
        variable_part.insert(std::pair{term->to_hash(), term});
    }

    if (get_variable_terms().size() == 0)
        return std::make_shared<ASTIntegerConstantNode>(get_constant_term());

    remove_zeros();
    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
};

std::shared_ptr<ASTNodeBase> ASTAdditionNode::expand()
{    
    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::shared_ptr<ASTNodeBase> expanded_operand;
    std::vector<std::multimap<std::string, std::shared_ptr<ASTNodeBase>>::iterator> terms_to_delete;
    std::vector<std::shared_ptr<ASTNodeBase>> new_terms;

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        std::tie(key, value) = *it;
        expanded_operand = value->expand();
        if (expanded_operand->get_type() == ASTNodeType::Addition) // Distributivity law applied
        {
            terms_to_delete.push_back(it);
            for (auto op : expanded_operand->get_operands())
            {
                // Check constant - subexpression degraded to a constant
                if (op->get_type() == ASTNodeType::IntegerConstant)
                {
                    set_constant_term(get_constant_term() + op->get_constant_term());
                    continue;
                }

                // If such term is present, increase its multiplication factor
                if (auto it = variable_part.find(op->to_hash()); it != variable_part.end())
                {
                    std::tie(key, value) = *it;
                    value->set_constant_term(value->get_constant_term() + op->get_constant_term());
                    continue;
                }

                // Adding a new term
                new_terms.push_back(op);
            }
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
        return std::make_shared<ASTIntegerConstantNode>(get_constant_term());

    remove_zeros();
    upadte_expression_string();
    upadte_hash_string();
    return shared_from_this();
}

std::string ASTAdditionNode::_to_string() const
{
    std::stringstream ss;



    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        ss << (*it).second->_to_string() << ((it == (--variable_part.end())) ? "" : "+");
    }

    int value = get_constant_term();
    if (value < 0)
        ss << "-" << std::to_string(-value);
    else if (value > 0)
        ss << "+" << std::to_string(value);
    else if (variable_part.size() == 0){
        ss << "0";
    }

    return ss.str();
}

std::string ASTAdditionNode::_to_hash() const
{
    std::stringstream ss;

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        ss << (*it).second->_to_hash() << ((it == (--variable_part.end())) ? "" : "+");
    }

    int value = get_constant_term();
    if (value < 0)
        ss << "-" << std::to_string(-value);
    else if (value > 0)
        ss << "+" << std::to_string(value);

    return ss.str();
}

// Called whenever a new operand is added to the operand vector
void ASTAdditionNode::process_operand(std::shared_ptr<ASTNodeBase> op)
{
    ASTNodeType type = op->get_type();
    switch (type)
    {
    case ASTNodeType::AccessPath: // Access Paths are transformed into multiplication 1 * access Path
    {
        // Update if factor if an access path is already present
        if (auto it = variable_part.find(op->to_hash()); it != variable_part.end())
        {
            std::string key;
            std::shared_ptr<ASTNodeBase> value;
            std::tie(key, value) = *it;
            value->set_constant_term(value->get_constant_term() + 1);
        }
        else
        {
            std::shared_ptr<ASTNodeBase> new_term = std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({op}));
            variable_part[new_term->to_hash()] = new_term;
        }
        break;
    }
    case ASTNodeType::Multiplication:
    {
        // Multiplication degraded to constant
        if (op->get_variable_terms().size() == 0){
            set_constant_term(get_constant_term() + op->get_constant_term());
        }
        else if (auto it = variable_part.find(op->to_hash()); it != variable_part.end())
        {
            std::string key;
            std::shared_ptr<ASTNodeBase> value;
            std::tie(key, value) = *it;
            value->set_constant_term(value->get_constant_term() + op->get_constant_term());
        }
        else
        {
            variable_part[op->to_hash()] = op;
        }
        break;
    }
    case ASTNodeType::Max: // Represented as c * min, c * max
    case ASTNodeType::Min:
        if (auto it = variable_part.find(op->to_hash()); it != variable_part.end())
        {
            std::string key;
            std::shared_ptr<ASTNodeBase> value;
            std::tie(key, value) = *it;
            value->set_constant_term(value->get_constant_term() + 1);
        }
        else
        {
            std::shared_ptr<ASTNodeBase> new_term = std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({op}));
            variable_part[new_term->to_hash()] = new_term;
        }
        break;
    case ASTNodeType::IntegerConstant:
        set_constant_term(get_constant_term() + op->get_constant_term());
        break;
    case ASTNodeType::Addition:
        for (const auto &add_op : op->get_operands())
        {
            process_operand(add_op);
        }
        break;
    }
}

// Variable
std::vector<std::shared_ptr<ASTNodeBase>> ASTAdditionNode::get_variable_terms() const
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
std::vector<std::shared_ptr<ASTNodeBase>> ASTAdditionNode::get_operands() const
{
    std::vector<std::shared_ptr<ASTNodeBase>> ops;

    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    for (const auto &item : variable_part)
    {
        std::tie(key, value) = item;
        ops.push_back(value);
    }
    if (constant_part->get_constant_term() != 0)
    {
        ops.push_back(constant_part);
    }
    return ops;
}


// Operations
 std::shared_ptr<ASTAdditionNode> operator+(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp)
{
    std::shared_ptr<ASTAdditionNode> result = std::dynamic_pointer_cast<ASTAdditionNode>(lexp->copy());
    std::shared_ptr<ASTAdditionNode> rexp_copy = std::dynamic_pointer_cast<ASTAdditionNode>(rexp->copy());
    // add
    for (const auto op : rexp_copy->get_operands())
    {
        result->process_operand(op);
    }
    result->remove_zeros();
    result->upadte_expression_string();
    result->upadte_hash_string();
    return result;
}

 std::shared_ptr<ASTAdditionNode> operator+=(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp)
{
    std::shared_ptr<ASTAdditionNode> rexp_copy = std::dynamic_pointer_cast<ASTAdditionNode>(rexp->copy());
    // add
    for (const auto op : rexp_copy->get_operands())
    {
        lexp->process_operand(op);
    }

    lexp->remove_zeros();
    lexp->upadte_expression_string();
    lexp->upadte_hash_string();
    return lexp;
}

 std::shared_ptr<ASTAdditionNode> operator-(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp)
{
    std::shared_ptr<ASTAdditionNode> result = std::dynamic_pointer_cast<ASTAdditionNode>(lexp->copy());
    std::shared_ptr<ASTAdditionNode> rexp_copy = std::dynamic_pointer_cast<ASTAdditionNode>(rexp->copy());
    // negate and add
    for (const auto op : rexp_copy->get_operands())
    {
        op->set_constant_term(-op->get_constant_term());
        result->process_operand(op);
    }
    result->remove_zeros();
    result->upadte_expression_string();
    result->upadte_hash_string();
    return result;
}

 std::shared_ptr<ASTAdditionNode> operator-=(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp)
{
    std::shared_ptr<ASTAdditionNode> rexp_copy = std::dynamic_pointer_cast<ASTAdditionNode>(rexp->copy());
    // negate and add
    for (const auto op : rexp_copy->get_operands())
    {
        op->set_constant_term(-op->get_constant_term());
        lexp->process_operand(op);
    }
    lexp->remove_zeros();
    lexp->upadte_expression_string();
    lexp->upadte_hash_string();
    return lexp;
}

 std::shared_ptr<ASTAdditionNode> operator-(const std::shared_ptr<ASTAdditionNode> &exp)
{
    std::shared_ptr<ASTAdditionNode> result = std::dynamic_pointer_cast<ASTAdditionNode>(exp->copy());
    // negate
    for (const auto op : result->get_operands())
    {
        op->set_constant_term(-op->get_constant_term());
    }
    result->upadte_expression_string();
    result->upadte_hash_string();
    return result;
}

 bool operator==(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp)
{
    if (lexp->variable_part.size() != rexp->variable_part.size())
    {
        return false;
    }

    // Check const
    if (lexp->get_constant_term() != rexp->get_constant_term())
    {
        return false;
    }

    std::string l_key, r_key;
    std::shared_ptr<ASTNodeBase> l_value, r_value;
    for (auto l_it = lexp->variable_part.begin(); l_it != lexp->variable_part.end(); l_it++)
    {
        std::tie(l_key, l_value) = *l_it;
        auto r_it = rexp->variable_part.find(l_key);

        if ((r_it == rexp->variable_part.end()))
        {
            return false;
        }

        std::tie(r_key, r_value) = *r_it;
        if (l_value->get_constant_term() != r_value->get_constant_term())
        {
            return false;
        }
    }

    return true;
}

void ASTAdditionNode::remove_zeros()
{
    std::string key;
    std::shared_ptr<ASTNodeBase> value;
    std::vector<std::multimap<std::string, std::shared_ptr<ASTNodeBase>>::iterator> terms_to_delete;

    for (auto it = variable_part.begin(); it != variable_part.end(); it++)
    {
        std::tie(key, value) = *it;
        if (value->get_constant_term() == 0)
        {
            terms_to_delete.push_back(it);
        }
    }

    for (auto it : terms_to_delete)
    {
        variable_part.erase(it);
    }
}
