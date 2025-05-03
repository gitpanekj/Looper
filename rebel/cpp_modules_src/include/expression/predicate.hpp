#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <memory>
#include <iostream>
#include <exception>

#include "expression.hpp"

class Predicate
{
protected:
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;
    Predicate(std::shared_ptr<Expression> _lhs, std::shared_ptr<Expression> _rhs) : lhs(_lhs), rhs(_rhs) {};

public:
    std::shared_ptr<Expression> getLHS() { return lhs; };
    std::shared_ptr<Expression> getRHS() { return rhs; };

    std::shared_ptr<Predicate> negate() const
    {
        return getNegation(lhs->copy(), rhs->copy());
    }

    std::string to_string() const
    {
        return lhs->to_string() + getPredicate() + rhs->to_string();
    }

    virtual std::shared_ptr<Expression> get_norm() const = 0;

protected:
    virtual std::string getPredicate() const = 0;
    virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const = 0;
};

class Less : public Predicate
{
public:
    Less(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return "<"; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { return rhs-lhs;}
};
class LessOrEqual : public Predicate
{
public:
    LessOrEqual(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return "<="; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { return rhs-lhs+Expression::create_constant(1); }
};
class Greater : public Predicate
{
public:
    Greater(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return ">"; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { return lhs-rhs;}
};
class GreaterOrEqual : public Predicate
{
public:
    GreaterOrEqual(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return ">="; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { return lhs-rhs+Expression::create_constant(1); }
};
class Equal : public Predicate
{
public:
    Equal(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return "=="; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { throw std::exception(); }
};
class NotEqual : public Predicate
{
public:
    NotEqual(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) : Predicate(lhs, rhs) {};
    std::string getPredicate() const override { return "!="; }
    std::shared_ptr<Predicate> getNegation(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) const override;
    std::shared_ptr<Expression> get_norm() const override { throw std::exception(); }
};

#endif
