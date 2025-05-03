#include "expression/expression.hpp"

std::vector<std::shared_ptr<ASTNodeBase>> try_expand_max_in_max(std::shared_ptr<ASTNodeBase> sum_expr)
{
    // c*var where c > 0
    if (sum_expr->get_variable_terms().size() != 1 || sum_expr->get_variable_terms()[0]->get_type() != ASTNodeType::Multiplication)
        return std::vector<std::shared_ptr<ASTNodeBase>>();
    auto mul_term = sum_expr->get_variable_terms()[0];

    if (mul_term->get_variable_terms().size() != 1 || 
        mul_term->get_variable_terms()[0]->get_type() != ASTNodeType::Max ||
        mul_term->get_constant_term() <= 0)
            return std::vector<std::shared_ptr<ASTNodeBase>>();

    // c*max(a,b,c,d)
    auto max = mul_term->get_variable_terms()[0];
    auto c = std::make_shared<ASTIntegerConstantNode>(mul_term->get_constant_term());
    std::vector<std::shared_ptr<ASTNodeBase>> expanded_terms;
    for (auto term : max->get_operands()){
        expanded_terms.push_back(std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({term->copy(), c->copy()})));
    }
    
    // c*a + c*b + c*b + c*d
    return expanded_terms;
}

std::vector<std::shared_ptr<ASTNodeBase>> try_expand_min_in_min(std::shared_ptr<ASTNodeBase> sum_expr)
{
    // c*var where c > 0
    if (sum_expr->get_variable_terms().size() != 1 || sum_expr->get_variable_terms()[0]->get_type() != ASTNodeType::Multiplication)
        return std::vector<std::shared_ptr<ASTNodeBase>>();
    auto mul_term = sum_expr->get_variable_terms()[0];

    if (mul_term->get_variable_terms().size() != 1 || 
        mul_term->get_variable_terms()[0]->get_type() != ASTNodeType::Min ||
        mul_term->get_constant_term() <= 0)
        return std::vector<std::shared_ptr<ASTNodeBase>>();

    // c*min(a,b,c,d)
    auto min = mul_term->get_variable_terms()[0];
    auto c = std::make_shared<ASTIntegerConstantNode>(mul_term->get_constant_term());
    std::vector<std::shared_ptr<ASTNodeBase>> expanded_terms;
    for (auto term : min->get_operands()){
        expanded_terms.push_back(std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({term->copy(), c->copy()})));
    }

    return expanded_terms;
}



std::shared_ptr<Expression> Expression::create_constant(int value)
{
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>());
    ast->set_constant_term(value);
    return std::make_shared<Expression>(ast);
}

std::shared_ptr<Expression> Expression::create_variable(std::string name)
{
    auto variable = std::make_shared<ASTAccessPathNode>(name);
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({variable}));
    return std::make_shared<Expression>(ast);
}

std::shared_ptr<Expression> Expression::create_addition(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
{
    return lhs + rhs;
}

std::shared_ptr<Expression> Expression::create_subtraction(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
{
    return lhs - rhs;
}

std::shared_ptr<Expression> Expression::create_negation(std::shared_ptr<Expression> exp)
{
    return -exp;
}

std::shared_ptr<Expression> Expression::create_multiplication(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
{
    return lhs * rhs;
}


std::shared_ptr<Expression> Expression::create_max(std::vector<std::shared_ptr<Expression>> terms)
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms_copy;
    for (int i = 0; i < terms.size(); i++)
    {
        auto term = terms[i];
        // If the term is in the form of c*max(a,b) and c > 0
        // then append c*a, c*b
        auto ast = term->get_ast_copy();
        auto expanded_terms = try_expand_max_in_max(ast);
        if (expanded_terms.size() == 0){
            terms_copy.push_back(ast);
            continue;
        }
        for (auto expanded_term : expanded_terms)
        {
            terms.push_back(std::make_shared<Expression>(expanded_term));
        }
    }

    std::shared_ptr<ASTNodeBase> max = std::make_shared<ASTMaxNode>(terms_copy);
    
    // Reduce to constant
    if (max->get_operands().size() == 1)
    {
        max = max->get_operands()[0];
    }
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({max}));
    return std::make_shared<Expression>(ast);
}

std::shared_ptr<Expression> Expression::create_min(std::vector<std::shared_ptr<Expression>> terms)
{
    std::vector<std::shared_ptr<ASTNodeBase>> terms_copy;
    for (int i = 0; i < terms.size(); i++)
    {
        auto term = terms[i];
        // If the term is in the form of c*max(a,b) and c > 0
        // then append c*a, c*b
        auto ast = term->get_ast_copy();
        auto expanded_terms = try_expand_min_in_min(ast);
        if (expanded_terms.size() == 0){
            terms_copy.push_back(ast);
            continue;
        }
        for (auto expanded_term : expanded_terms)
        {
            terms.push_back(std::make_shared<Expression>(expanded_term));
        }
    }

    std::shared_ptr<ASTNodeBase> max = std::make_shared<ASTMinNode>(terms_copy);
    
    // Reduce to constant
    if (max->get_operands().size() == 1)
    {
        max = max->get_operands()[0];
    }
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({max}));
    return std::make_shared<Expression>(ast);
}


void Expression::expand()
{
    auto subs_ast = expr_ast->expand();
    if (subs_ast->get_type() != ASTNodeType::Addition)
    {
        expr_ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({subs_ast}));
    }
    else
    {
        expr_ast = std::dynamic_pointer_cast<ASTAdditionNode>(subs_ast);
    }
}

void Expression::substitute(std::string name, std::shared_ptr<Expression> expr)
{
    auto subs_ast = expr_ast->substitute(name, expr->expr_ast);
    if (subs_ast->get_type() != ASTNodeType::Addition)
    {
        expr_ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({subs_ast}));
    }
    else
    {
        expr_ast = std::dynamic_pointer_cast<ASTAdditionNode>(subs_ast);
    }
}

/********* Overloaded operators *********/
std::shared_ptr<Expression> operator+(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    return std::make_shared<Expression>(lexp->expr_ast + rexp->expr_ast);
}

std::shared_ptr<Expression> operator+=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    lexp->expr_ast += rexp->expr_ast;
    return lexp;
}

std::shared_ptr<Expression> operator-(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    return std::make_shared<Expression>(lexp->expr_ast - rexp->expr_ast);
}

std::shared_ptr<Expression> operator-=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    lexp->expr_ast -= rexp->expr_ast;
    return lexp;
}

std::shared_ptr<Expression> operator*(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto mul = std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({lexp->expr_ast->copy(), rexp->expr_ast->copy()}));
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({mul}));
    return std::make_shared<Expression>(ast);
}

std::shared_ptr<Expression> operator*=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto mul = std::make_shared<ASTMultiplicationNode>(std::vector<std::shared_ptr<ASTNodeBase>>({lexp->expr_ast, rexp->expr_ast->copy()}));
    auto ast = std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({mul}));
    return std::make_shared<Expression>(ast);
}

std::shared_ptr<Expression> operator-(const std::shared_ptr<Expression> &exp)
{
    return std::make_shared<Expression>(-exp->expr_ast);
}

std::shared_ptr<Expression> operator--(std::shared_ptr<Expression> &exp, int)
{
    exp->expr_ast->set_constant_term(exp->expr_ast->get_constant_term() - 1);
    return exp;
}

std::shared_ptr<Expression> operator++(std::shared_ptr<Expression> &exp, int)
{
    exp->expr_ast->set_constant_term(exp->expr_ast->get_constant_term() + 1);
    return exp;
}

std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Expression> obj)
{
    os << obj->expr_ast->to_string();
    return os;
}

bool operator==(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    if (lexp.get() == rexp.get()){return true;}
    return lexp->expr_ast == rexp->expr_ast;
}

bool operator!=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    return !(lexp->expr_ast == rexp->expr_ast);
}

bool operator<(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto dif = lexp - rexp;
    return (dif->expr_ast->get_variable_terms().size() == 0) && dif->expr_ast->get_constant_term() < 0;
}

bool operator<=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto dif = lexp - rexp;
    return (dif->expr_ast->get_variable_terms().size() == 0) && dif->expr_ast->get_constant_term() <= 0;
}

bool operator>(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto dif = rexp - lexp;
    return (dif->expr_ast->get_variable_terms().size() == 0) && dif->expr_ast->get_constant_term() < 0;
}

bool operator>=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp)
{
    auto dif = rexp - lexp;
    return (dif->expr_ast->get_variable_terms().size() == 0) && dif->expr_ast->get_constant_term() <= 0;
}