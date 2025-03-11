#include "expression/expression.hpp"

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