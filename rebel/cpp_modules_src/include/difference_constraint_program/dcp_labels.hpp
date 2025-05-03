#ifndef DCP_LABELS_H
#define DCP_LABELS_H

#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "expression/expression.hpp"

namespace difference_constraint_program {
struct DifferenceConstraint {
    std::shared_ptr<Expression> x,y,c; //< x <= y + c where c is constant and x,y are expression over program variables

    DifferenceConstraint() : x(nullptr), y(nullptr), c(nullptr) {}
    DifferenceConstraint(std::shared_ptr<Expression> _x,
                         std::shared_ptr<Expression> _y,
                         std::shared_ptr<Expression> _c) : x(_x), y(_y), c(_c) {}

    std::string to_string() const
    {
        std::stringstream ss;
        ss << x->to_string() << " <= " << "(" << y->to_string() << ")" << " + " << c->to_string();
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const DifferenceConstraint& obj)
    {
        os << obj.to_string();
        return os;
    }
};


struct DCPTransitionLabel {
    bool true_branch;
    std::unordered_map<std::string, std::shared_ptr<Expression>> guards;
    std::unordered_map<std::string, DifferenceConstraint> constraints;

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "Guards: (";
        for (const auto &[_, exp] : guards){
            ss << exp->to_string() << ", ";
        }
        ss << ")\n";

        for (const auto &[key, constraint] : constraints){
            ss << constraint << std::endl;
        }
        return ss.str();
    }

    void add_dc(DifferenceConstraint dc){
        constraints[dc.x->to_string()] = dc;
    }

    void add_guard(std::shared_ptr<Expression> guard){
        guards[guard->to_string()] =  guard;
    }

    bool is_in_guards(std::shared_ptr<Expression> guard){
        return guards.find(guard->to_string()) != guards.end();
    }
};

}


#endif