#include "expression/expression.hpp"

namespace expression
{

    std::shared_ptr<Predicate> Less::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<GreaterOrEqual>(lhs, rhs);
    }

    std::shared_ptr<Predicate> LessOrEqual::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<Greater>(lhs, rhs);
    }

    std::shared_ptr<Predicate> Greater::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<LessOrEqual>(lhs, rhs);
    }

    std::shared_ptr<Predicate> GreaterOrEqual::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<Less>(lhs, rhs);
    }

    std::shared_ptr<Predicate> Equal::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<NotEqual>(lhs, rhs);
    }

    std::shared_ptr<Predicate> NotEqual::getNegation(std::shared_ptr<Term> lhs, std::shared_ptr<Term> rhs) const
    {
        return std::make_shared<Equal>(lhs, rhs);
    }
}