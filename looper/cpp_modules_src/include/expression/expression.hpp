#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "expression_ast.hpp"

/**
 * @brief Class representing algebraic epxression.
 * 
 * Thre Expression class provides interface for expression manipulation
 * and encapsulates the abstract syntax tree representation of an algebraic
 * expression.
 * 
 * An algebraic expression is represented as a sum of multiplications.
 */
class Expression
{
private:
    std::shared_ptr<ASTAdditionNode> expr_ast; //< Root addition node of an algebraic expression
public:
    Expression(std::shared_ptr<ASTNodeBase> ast) : expr_ast(std::make_shared<ASTAdditionNode>(std::vector<std::shared_ptr<ASTNodeBase>>({ast}))) {}
    /********* factory methods *********/
    static std::shared_ptr<Expression> create_constant(int value);
    static std::shared_ptr<Expression> create_variable(std::string name);
    static std::shared_ptr<Expression> create_addition(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    static std::shared_ptr<Expression> create_subtraction(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    static std::shared_ptr<Expression> create_negation(std::shared_ptr<Expression>exp);
    static std::shared_ptr<Expression> create_multiplication(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);
    static std::shared_ptr<Expression> create_max(std::vector<std::shared_ptr<Expression>> terms);
    static std::shared_ptr<Expression> create_min(std::vector<std::shared_ptr<Expression>> terms);

    /********* Expression manipulation *********/
    std::shared_ptr<Expression> copy() { return std::make_shared<Expression>(expr_ast->copy()); }
    std::shared_ptr<ASTNodeBase> get_ast_copy() { return expr_ast->copy(); }
    void expand();
    void substitute(std::string name, std::shared_ptr<Expression> expr);
    bool is_constant() const { return expr_ast->get_variable_terms().size() == 0; }
    int separate_constant() { return expr_ast->get_constant_term();}
    std::unordered_set<std::string> get_variable_names() {
        std::unordered_set<std::string> names;
        expr_ast->get_variable_names(names);
        return names;
    }
    void rename_variable(std::string from, std::string to){
        substitute(from, Expression::create_variable(to));
    }

    /********* Overloaded operators *********/
    friend std::shared_ptr<Expression> operator-(const  std::shared_ptr<Expression> &exp);
    friend std::shared_ptr<Expression> operator--(std::shared_ptr<Expression> &exp, int);
    friend std::shared_ptr<Expression> operator++(std::shared_ptr<Expression> &exp, int);
    friend std::shared_ptr<Expression> operator+(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend std::shared_ptr<Expression> operator+=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend std::shared_ptr<Expression> operator-(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend std::shared_ptr<Expression> operator-=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend std::shared_ptr<Expression> operator*(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend std::shared_ptr<Expression> operator*=(std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    
    friend bool operator==(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend bool operator!=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend bool operator<(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend bool operator<=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend bool operator>(const  std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);
    friend bool operator>=(const std::shared_ptr<Expression> &lexp, const std::shared_ptr<Expression> &rexp);


    
    /********* String representation *********/
    std::string to_string() { return expr_ast->to_string(); }
    std::string to_hash() { return expr_ast->to_hash(); }
    friend std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Expression> obj);
};


#endif