/**
 * @file expression_ast.hpp
 * @author Jan Pánek xpanek11@stud.fit.vutbr.cz
 * @brief Nodes of abstract syntax tree representing algebraic expression.
 * @version 0.1
 * @date 2025-02-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef EXPR_AST_H
#define EXPR_AST_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <iostream>

enum class ASTNodeType
{
    // Atomic nodes
    AccessPath,
    IntegerConstant,
    // Operations
    Addition,
    Multiplication,
};

/**
 * @brief Abstract base class for a expression AST nodes.
 */
class ASTNodeBase : public std::enable_shared_from_this<ASTNodeBase>
{
private:
    ASTNodeType type;              //< Node type
    std::string hash_string;       //< string used for hash computation
    std::string expression_string; //< string representation of the expression

protected:
    void upadte_expression_string() { expression_string = _to_string(); } //< Update the string representation after AST modification
    void upadte_hash_string() { hash_string = _to_hash(); }               //< Update the hash after AST modification

public:
    ASTNodeBase(ASTNodeType t) : type(t), expression_string(""), hash_string("") {}
    virtual ~ASTNodeBase() = default;
    virtual std::shared_ptr<ASTNodeBase> copy() = 0;                                                          //< Return a pointer to a deep copy of an AST starting from the AST node
    virtual std::shared_ptr<ASTNodeBase> expand() = 0;                                                        //< Return a pointer to an AST after application of distribution law
    virtual std::shared_ptr<ASTNodeBase> substitute(std::string name, std::shared_ptr<ASTNodeBase> expr) = 0; //< Return a pointer to an AST with variables with give name sibstituted with expr

    /**************  Getters, setters **************/
    ASTNodeType get_type() const { return type; }               //< Return an AST node type
    std::string to_hash() const { return hash_string; }         //< Return an AST node hash
    std::string to_string() const { return expression_string; } //< Return an AST node string representation
    // Constant
    virtual int get_constant_term() const = 0;      //< Return the constant term of an AST node
    virtual void set_constant_term(int _value) = 0; //< Set a value of the constant term of an AST node
    virtual void get_variable_names(std::unordered_set<std::string> &names) const = 0;
    // Variables
    virtual std::vector<std::shared_ptr<ASTNodeBase>> get_variable_terms() const = 0; //< Get vector of terms involving variables
    // Operands
    virtual std::vector<std::shared_ptr<ASTNodeBase>> get_operands() const = 0; // Get a vector of both constant and variable terms

    // Repr
    virtual std::string _to_string() const = 0; //< Get a string representatiton of the AST
    virtual std::string _to_hash() const = 0;   //< Get a string used for hashing
    friend std::ostream &operator<<(std::ostream &os, const ASTNodeBase &obj)
    {
        os << obj.expression_string;
        return os;
    }
};

/**
 * @brief AST node representing integer constant.
 */
class ASTIntegerConstantNode : public ASTNodeBase
{
private:
    int value;

public:
    ASTIntegerConstantNode(int value) : ASTNodeBase(ASTNodeType::IntegerConstant), value(value)
    {
        upadte_expression_string();
        upadte_hash_string();
    }
    std::shared_ptr<ASTNodeBase> copy() override { return std::make_shared<ASTIntegerConstantNode>(value); }
    std::shared_ptr<ASTNodeBase> expand() override { return shared_from_this(); }
    std::shared_ptr<ASTNodeBase> substitute(std::string name, std::shared_ptr<ASTNodeBase> expr) override { return shared_from_this(); }

    /**************  Getters, setters **************/
    // Constant
    int get_constant_term() const override { return value; }
    void set_constant_term(int _value) override { value = _value; }
    void get_variable_names(std::unordered_set<std::string> &names) const override {}
    // Variables
    std::vector<std::shared_ptr<ASTNodeBase>> get_variable_terms() const override { return std::vector<std::shared_ptr<ASTNodeBase>>(); }
    // Operands
    std::vector<std::shared_ptr<ASTNodeBase>> get_operands() const override { return std::vector<std::shared_ptr<ASTNodeBase>>(); }

    // Repr
    std::string _to_string() const override { return std::to_string(value); }
    std::string _to_hash() const override { return std::to_string(value); }
};

/**
 * @brief AST node representing access path
 */
class ASTAccessPathNode : public ASTNodeBase
{
private:
    std::string name;

public:
    ASTAccessPathNode(std::string name) : ASTNodeBase(ASTNodeType::AccessPath), name(name)
    {
        upadte_expression_string();
        upadte_hash_string();
    }
    std::shared_ptr<ASTNodeBase> copy() override { return std::make_shared<ASTAccessPathNode>(name); }
    std::shared_ptr<ASTNodeBase> expand() override { return shared_from_this(); };
    std::shared_ptr<ASTNodeBase> substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr) override
    {
        // not matching name
        if (_name != name)
        {
            return shared_from_this();
        }

        // Substituted expression is addition with single operand (variable or constant)
        // NOTE: required since all the expressions at the top level are represented as addition of multiplications
        if (expr->get_type() == ASTNodeType::Addition && expr->get_operands().size() == 1)
        {
            return expr->get_operands()[0];
        }

        // Substituted expression is addition with no operand (equivalent of 0 term)
        // NOTE: required since all the expressions at the top level are represented as addition of multiplications
        if (expr->get_type() == ASTNodeType::Addition && expr->get_operands().size() == 0) // 0
        {
            return std::make_shared<ASTIntegerConstantNode>(0);
        }

        return expr; // substituted expression
    }

    /**************  Getters and setters **************/
    // Constant
    int get_constant_term() const override { return 0; }
    void set_constant_term(int _value) override {}
    void get_variable_names(std::unordered_set<std::string> &names) const override { names.insert(name); }
    // Variables
    std::vector<std::shared_ptr<ASTNodeBase>> get_variable_terms() const override { return std::vector<std::shared_ptr<ASTNodeBase>>(); }
    // Operands
    std::vector<std::shared_ptr<ASTNodeBase>> get_operands() const override { return std::vector<std::shared_ptr<ASTNodeBase>>(); }

    // Repr
    std::string _to_string() const override { return name; }
    std::string _to_hash() const override { return name; }
};


/**
 * @brief AST node representing multiplication operation.
 */
class ASTMultiplicationNode : public ASTNodeBase
{
private:
    std::multimap<std::string, std::shared_ptr<ASTNodeBase>> variable_part;
    std::shared_ptr<ASTIntegerConstantNode> constant_part;

protected:
    void process_operand(std::shared_ptr<ASTNodeBase> op);

public:
    ASTMultiplicationNode(std::vector<std::shared_ptr<ASTNodeBase>> ops);
    std::shared_ptr<ASTNodeBase> copy() override;
    std::shared_ptr<ASTNodeBase> expand() override;
    std::shared_ptr<ASTNodeBase> substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr) override;

    /**************  Getters, setters **************/
    // Constant
    int get_constant_term() const override { return constant_part->get_constant_term(); }
    void set_constant_term(int _value) override { constant_part->set_constant_term(_value); }
    void get_variable_names(std::unordered_set<std::string> &names) const override {
        for (auto it = variable_part.begin(); it != variable_part.end(); it++)
        {
            it->second->get_variable_names(names);
        }
    }
    // Variables
    std::vector<std::shared_ptr<ASTNodeBase>> get_variable_terms() const override;
    // Operands
    std::vector<std::shared_ptr<ASTNodeBase>> get_operands() const override;

    // Repr
    std::string _to_string() const override;
    std::string _to_hash() const override;
};



/**
 * @brief AST node representing addition operation.
 */
class ASTAdditionNode : public ASTNodeBase
{
private:
    std::map<std::string, std::shared_ptr<ASTNodeBase>> variable_part;
    std::shared_ptr<ASTIntegerConstantNode> constant_part;

protected:
    void process_operand(std::shared_ptr<ASTNodeBase> op);
    void remove_zeros();

public:
    ASTAdditionNode(std::vector<std::shared_ptr<ASTNodeBase>> ops);
    std::shared_ptr<ASTNodeBase> copy() override;
    std::shared_ptr<ASTNodeBase> expand() override;
    std::shared_ptr<ASTNodeBase> substitute(std::string _name, std::shared_ptr<ASTNodeBase> expr) override;

    /**************  Getters, setters **************/
    // Constant
    int get_constant_term() const override { return constant_part->get_constant_term(); }
    void set_constant_term(int _value) override { constant_part->set_constant_term(_value); }
    void get_variable_names(std::unordered_set<std::string> &names) const override {
        for (auto it = variable_part.begin(); it != variable_part.end(); it++)
        {
            it->second->get_variable_names(names);
        }
    }
    // Variable
    std::vector<std::shared_ptr<ASTNodeBase>> get_variable_terms() const override;
    // Operands
    std::vector<std::shared_ptr<ASTNodeBase>> get_operands() const override;

    // Repr
    std::string _to_string() const override;
    std::string _to_hash() const override;

    // Operations
    friend std::shared_ptr<ASTAdditionNode> operator+(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp);
    friend std::shared_ptr<ASTAdditionNode> operator+=(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp);
    friend std::shared_ptr<ASTAdditionNode> operator-(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp);
    friend std::shared_ptr<ASTAdditionNode> operator-=(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp);
    friend std::shared_ptr<ASTAdditionNode> operator-(const std::shared_ptr<ASTAdditionNode> &exp);
    friend bool operator==(const std::shared_ptr<ASTAdditionNode> &lexp, const std::shared_ptr<ASTAdditionNode> &rexp);

};





#endif