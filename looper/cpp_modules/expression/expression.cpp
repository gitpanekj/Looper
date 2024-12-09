#include "expression/expression.hpp"
#include <string>
#include <memory>

namespace expression {

std::string Constant::toStr() const 
{
    return std::to_string(getValue());
}


std::string AccessPath::toStr() const 
{
    return name;
}


std::string Add::toStr() const 
{
    return "(" + getFirstOperand().toStr() + " + " + getSecondOperand().toStr() + ")";
}


std::string Sub::toStr() const 
{
    return "(" + getFirstOperand().toStr() + " - " + getSecondOperand().toStr() + ")";
}

std::string Mul::toStr() const 
{
    return getFirstOperand().toStr() + " * " + getSecondOperand().toStr();
}





std::shared_ptr<Expression> Constant::replace(const std::string name, std::shared_ptr<Expression> expr)
{
    return std::make_shared<Constant>(*this);
}


std::shared_ptr<Expression> AccessPath::replace(const std::string name, std::shared_ptr<Expression> expr)
{
    if (this->name == name)
    {
        return std::move(expr);
    }

    return std::make_shared<AccessPath>(*this);
}


std::shared_ptr<Expression> Add::replace(const std::string name, std::shared_ptr<Expression> expr)
{
    std::shared_ptr<Expression> newLeft = getFirstOperand().replace(name, expr);
    std::shared_ptr<Expression> newRight = getSecondOperand().replace(name, expr);
    setFirstOperand(newLeft);
    setSecondOperand(newRight);

    return std::make_shared<Add>(newLeft, newRight);
}


std::shared_ptr<Expression> Sub::replace(const std::string name, std::shared_ptr<Expression> expr)
{
    std::shared_ptr<Expression> newLeft = getFirstOperand().replace(name, expr);
    std::shared_ptr<Expression> newRight = getSecondOperand().replace(name, expr);
    setFirstOperand(newLeft);
    setSecondOperand(newRight);

    return std::make_shared<Sub>(newLeft, newRight);;
}

std::shared_ptr<Expression> Mul::replace(const std::string name, std::shared_ptr<Expression> expr)
{
    std::shared_ptr<Expression> newLeft = getFirstOperand().replace(name, expr);
    std::shared_ptr<Expression> newRight = getSecondOperand().replace(name, expr);
    setFirstOperand(newLeft);
    setSecondOperand(newRight);

    return std::make_shared<Mul>(newLeft, newRight);;
}
}
