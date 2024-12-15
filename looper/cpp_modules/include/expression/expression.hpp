#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <memory>
#include <iostream>

namespace expression
{
    class Term
    {
    public:
        virtual ~Term() = default;
        virtual std::shared_ptr<Term> replace(const std::string name, std::shared_ptr<Term> exp) = 0;
        virtual std::string toStr() const = 0;
        virtual std::shared_ptr<Term> deepCopy() const = 0;
    };

    class Predicate
    {
    protected:
        std::shared_ptr<Term> lhs;
        std::shared_ptr<Term> rhs;
        Predicate(std::shared_ptr<Term> _lhs, std::shared_ptr<Term> _rhs) : lhs(_lhs), rhs(_rhs) {};

    public:
        std::shared_ptr<Term> getLHS() { return lhs; };
        std::shared_ptr<Term> getRHS() { return rhs; };

        std::shared_ptr<Predicate> negate() const
        {
            return getNegation(lhs->deepCopy(), rhs->deepCopy());
        }

        std::string toStr() const
        {
            return lhs->toStr() + getPredicate() + rhs->toStr();
        }

    protected:
        virtual std::string getPredicate() const = 0;
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const = 0;
    };

    class Less : public Predicate
    {
    public:
        Less(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return "<"; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };
    class LessOrEqual : public Predicate
    {
    public:
        LessOrEqual(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return "<="; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };
    class Greater : public Predicate
    {
    public:
        Greater(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return ">"; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };
    class GreaterOrEqual : public Predicate
    {
    public:
        GreaterOrEqual(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return ">="; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };
    class Equal : public Predicate
    {
    public:
        Equal(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return "=="; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };
    class NotEqual : public Predicate
    {
    public:
        NotEqual(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) : Predicate(lhs, rhs) {};
        virtual std::string getPredicate() const override { return "!="; }
        virtual std::shared_ptr<Predicate> getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const override;
    };

    class IntConstant : public Term, public std::enable_shared_from_this<IntConstant>
    {
    private:
        int value;

    public:
        IntConstant(int _value) : value(_value) {};

        virtual std::shared_ptr<Term> replace(const std::string name, std::shared_ptr<Term> exp) override { return shared_from_this(); };
        virtual std::string toStr() const override { return std::to_string(value); };
        virtual std::shared_ptr<Term> deepCopy() const override { return std::make_shared<IntConstant>(value); };
        int getValue() { return value; };
        void negate() { value = -value;};
    };

    class AccessPath : public Term, public std::enable_shared_from_this<AccessPath>
    {
    private:
        std::string access_path;

    public:
        AccessPath(std::string _access_path) : access_path(_access_path) {};

        virtual std::shared_ptr<Term> replace(const std::string name, std::shared_ptr<Term> exp) override
        {
            if (access_path == name)
            {
                return exp->deepCopy();
            }

            return shared_from_this();
        };
        virtual std::shared_ptr<Term> deepCopy() const override { return std::make_shared<AccessPath>(access_path); };
        virtual std::string toStr() const override { return access_path; };
        std::string getName() { return access_path; };
    };

    class BinaryOperation : public Term, public std::enable_shared_from_this<BinaryOperation>
    {
    protected:
        std::shared_ptr<Term> op1;
        std::shared_ptr<Term> op2;
        BinaryOperation(std::shared_ptr<Term> _op1, std::shared_ptr<Term> _op2) : op1(_op1), op2(_op2) {};

    public:
        virtual ~BinaryOperation() = default;
        virtual std::string toStr() const override { return "(" + op1->toStr() + getOperator() + op2->toStr() + ")"; };
        virtual std::shared_ptr<Term> replace(const std::string name, std::shared_ptr<Term> exp) override
        {
            op1 = op1->replace(name, exp);
            op2 = op2->replace(name, exp);

            return shared_from_this();
        }

        virtual std::shared_ptr<Term> deepCopy() const override
        {
            return createOperator(op1->deepCopy(), op2->deepCopy());
        };

    protected:
        virtual std::string getOperator() const = 0;
        virtual std::shared_ptr<BinaryOperation> createOperator(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) const = 0;
    };

    class Add : public BinaryOperation
    {
    public:
        Add(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) : BinaryOperation(op1, op2) {};

    protected:
        virtual std::string getOperator() const override { return "+"; };
        virtual std::shared_ptr<BinaryOperation> createOperator(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) const override
        {
            return std::make_shared<Add>(op1, op2);
        }
    };

    class Sub : public BinaryOperation
    {
    public:
        Sub(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) : BinaryOperation(op1, op2) {};

    protected:
        virtual std::string getOperator() const override { return "-"; };
        virtual std::shared_ptr<BinaryOperation> createOperator(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) const override
        {
            return std::make_shared<Sub>(op1, op2);
        }
    };


    class Mul : public BinaryOperation
    {
    public:
        Mul(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) : BinaryOperation(op1, op2) {};

    protected:
        virtual std::string getOperator() const override { return "*"; };
        virtual std::shared_ptr<BinaryOperation> createOperator(std::shared_ptr<Term> op1, std::shared_ptr<Term> op2) const override
        {
            return std::make_shared<Mul>(op1, op2);
        }
    };

}
#endif