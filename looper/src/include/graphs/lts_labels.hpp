#ifndef LABELS_H
#define LABELS_H

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include "expression/expression.hpp"
#include "expression/predicate.hpp"
#include <unordered_map>

/**
 * @brief Structure representing assignment on the LTS edge which changes a program state.
 *
 */
struct LTSTransitionAssignment
{
    std::string lhs;                 //< access path for memory cell the value is assigned to
    std::shared_ptr<Expression> rhs; //< assigned value
    LTSTransitionAssignment(std::string _lhs, std::shared_ptr<Expression> _rhs) : lhs(_lhs), rhs(_rhs) {};

    std::string to_string() const
    {
        std::stringstream ss;
        ss << lhs << " = " << rhs->to_string();
        return ss.str();
    }
};

/**
 * @brief Structure representing transition condition in DNF.
 *
 * (A and B and C) or (D and E) or (F) or ...
 * {{A,B,C},          {D,E},       {F},   ...}
 *
 */
struct LTSTransitionCondition
{
    bool true_branch;
    std::vector<std::vector<std::shared_ptr<Predicate>>> condition; //< condition in DNF (disjunction of conjuctions)
    LTSTransitionCondition(std::shared_ptr<Predicate> _condition, bool _true_branch) : true_branch(_true_branch)  { condition.push_back({_condition}); }
    LTSTransitionCondition(std::vector<std::shared_ptr<Predicate>> _compound_condition, bool _true_branch) : true_branch(_true_branch)  { condition.push_back(_compound_condition); }
    LTSTransitionCondition() : condition(), true_branch(false) {};                                                                                             //< empty vector of std::shared_ptr<Predicate>s
    void add_condition(std::shared_ptr<Predicate> _condition) { condition.push_back({_condition}); }                              //< Adding new term to disjunction list
    void add_condition(std::vector<std::shared_ptr<Predicate>> _compound_condition) { condition.push_back(_compound_condition); } //< Adding new conjuction to disjuctionl list

    std::vector<std::vector<std::shared_ptr<Expression>>> get_norm(){
        std::vector<std::vector<std::shared_ptr<Expression>>> norms;
        std::vector<std::shared_ptr<Expression>> cnf;
        for (const auto& cube : condition){
            cnf.clear();
            for (const auto& cond : cube){
                try {
                    cnf.push_back(cond->get_norm());
                }
                catch (const std::exception& e){}
            }
            norms.push_back(cnf);
        }
        return norms;
    }

    std::string to_string() const
    {
        if (condition.size() == 0){
            return "";
        }
    
        std::stringstream ss;
        for (auto cond_it = condition.begin(); cond_it != condition.end(); )
        {
            for (auto cnf_it = (*cond_it).begin(); cnf_it != (*cond_it).end();)
            {
                ss << (*cnf_it)->to_string();
                if (++cnf_it != (*cond_it).end())
                    ss << " and ";
            }
            if (++cond_it != condition.end())
                ss << " or ";
        }

        return ss.str();
    }
};

/**
 * @brief Structure representing LTS transition label.
 *
 * TODO: formal def.
 */
struct LTSTransitionLabel
{
    LTSTransitionCondition condition;                 //< transition condition in DNF
    std::vector<LTSTransitionAssignment> assignments; //< vector of transition assignments which change a program state

    LTSTransitionLabel(LTSTransitionCondition cond, std::vector<LTSTransitionAssignment> assignments) : condition(cond), assignments(assignments) {}
    LTSTransitionLabel(std::vector<LTSTransitionAssignment> assignments) : assignments(assignments) {}
    LTSTransitionLabel() {}

    std::string to_string() const
    {
        std::stringstream ss;

        ss << "Condition: (" << condition.to_string() << ")";
        ss << "\n";
        for (const auto &assignment : assignments)
        {
            ss << assignment.to_string();
            ss << "\n";
        }

        return ss.str();
    };
};

#endif