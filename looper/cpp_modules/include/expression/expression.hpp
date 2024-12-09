#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <memory>


namespace expression {
/****************** BASE CLASSES  ******************/
/**
 * @brief Expression representation using Abstract Syntax Tree.
 *
 */
class Expression
{
public:
    virtual ~Expression() = default;
    // virtual void simplify() = 0;
    // virtual bool equal(const Expression& other) const = 0; //TODO:
    virtual std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) = 0;
    virtual std::string toStr() const = 0;
};


class BinaryOperation
{
private:
    std::shared_ptr<Expression> op1;
    std::shared_ptr<Expression> op2;
public:
    BinaryOperation(std::shared_ptr<Expression> first, std::shared_ptr<Expression> second) : op1(std::move(first)), op2(std::move(second)) {}
    Expression& getFirstOperand() const {return *op1;}
    Expression& getSecondOperand() const {return *op2;}
    void setFirstOperand(std::shared_ptr<Expression> first)  {op1 = std::move(first);}
    void setSecondOperand(std::shared_ptr<Expression> second) {op2 = std::move(second);}
};
/****************** END OF BASE CLASSES  ******************/



/**
 * @brief
 *
 */
class Constant : public Expression
{
private:
    int value;

public:
    Constant(int value) : value(value) {}
    int getValue() const { return value; }
    std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) override;
    std::string toStr() const override;
};



/**
 * @brief
 *
 */
class AccessPath : public Expression
{
private:
    std::string name;

public:
    AccessPath(std::string name) : name(name) {}
    std::string getName() const { return name; }
    std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) override;
    std::string toStr() const override;
};



class Add : public Expression, public BinaryOperation
{
public:
    Add(std::shared_ptr<Expression> first, std::shared_ptr<Expression> second) : BinaryOperation(std::move(first), std::move(second)) {}
    std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) override;
    std::string toStr() const override;
};



class Sub : public Expression, public BinaryOperation
{
public:
    Sub(std::shared_ptr<Expression> first, std::shared_ptr<Expression> second) : BinaryOperation(std::move(first), std::move(second)) {}
    std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) override;
    std::string toStr() const override;
};



class Mul : public Expression, public BinaryOperation
{
public:
    Mul(std::shared_ptr<Expression> first, std::shared_ptr<Expression> second) : BinaryOperation(std::move(first), std::move(second)) {}
    std::shared_ptr<Expression> replace(const std::string name, std::shared_ptr<Expression> exp) override;
    std::string toStr() const override;
};
}
#endif

