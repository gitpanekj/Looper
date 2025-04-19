#ifndef LTS_EXECUTION_CONTEXT
#define LTS_EXECUTION_CONTEXT

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>

#include "expression/expression.hpp"
#include "expression/predicate.hpp"
#include "labeled_transition_system/lts_labels.hpp"
#include "lts_constructor/exceptions.hpp"

#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
using namespace labeled_transition_system;

struct TransitionExecutionContext
{
    std::unordered_map<std::string, std::shared_ptr<Expression>> expression_cache; //< mapping of temporary variables to expressions they represent
    std::unordered_map<std::string, std::shared_ptr<Predicate>> predicate_cache;   //< mapping of temporary variables to predicate they represent

    std::unordered_map<std::string, std::tuple<std::shared_ptr<Predicate>, bool>> block_name_to_predicate; //< mapping of the block names which are destination of jump instruction to predicate
    std::vector<LTSTransitionAssignment> statement_batch;                                                  //< vector of assignments which label an LTS edge
    LTSTransitionCondition pending_condition;                                                              //< condition labeling the next edge to be created

    std::unordered_set<std::string> invalidated_variables;  //< Set of variables whose value cannot be determined due to unsupported instruction

    llvm::BasicBlock* previous_basic_block = nullptr;

    void clear()
    {
        //expression_cache.clear();
        //predicate_cache.clear();
        statement_batch.clear();
        // block_name_to_predicate.clear();
        pending_condition = LTSTransitionCondition();
    }

    void check_target_node_of_jump(std::string basic_block_name)
    {
        auto it = block_name_to_predicate.find(basic_block_name);
        if (it != block_name_to_predicate.end())
        {
            bool true_branch;
            std::shared_ptr<Predicate> condition;
            std::tie(condition, true_branch) = it->second;
            pending_condition = LTSTransitionCondition(condition, true_branch);
            block_name_to_predicate.erase(basic_block_name);
        }
    }

    LTSTransitionLabel get_transition_label()
    {
        LTSTransitionLabel transition_label;
        std::vector<LTSTransitionAssignment> state_change;
        state_change = determine_program_state_change(statement_batch);
        if (pending_condition.condition.size() > 0)
        {
            transition_label = LTSTransitionLabel(pending_condition, state_change);
            transition_label.true_branch = pending_condition.true_branch;
        }
        else
        {
            transition_label = LTSTransitionLabel(LTSTransitionCondition(), state_change);
        }
        clear();
        return transition_label;
    }

    std::shared_ptr<Expression> get_operand(llvm::Value *operand)
    {
        // named variable (original or generated)
        if (operand->hasName())
        {
            std::string var_name = operand->getName().str();
            if (invalidated_variables.find(var_name) != invalidated_variables.end()){
                throw InvalidatedValue("Invalid operand");
            }
            auto it = expression_cache.find(var_name);
            if (it != expression_cache.end())
            {
                return it->second;
            }

            return Expression::create_variable(var_name);
        }

        // temporary variable
        if (llvm::isa<llvm::Instruction>(operand))
        {
            std::string temp_var_name = operand->getNameOrAsOperand();
            if (invalidated_variables.find(temp_var_name) != invalidated_variables.end()){
                throw InvalidatedValue("Invalid operand");
            }
            auto it = expression_cache.find(temp_var_name);
            if (it != expression_cache.end())
            {
                return it->second;
            }

            return Expression::create_variable(temp_var_name);
        }

        // constant
        if (llvm::isa<llvm::ConstantInt>(operand))
        {
            llvm::ConstantInt *const_int = (llvm::ConstantInt *)operand;
            return Expression::create_constant(const_int->getValue().getSExtValue());
        }

        // unknow
        return Expression::create_variable("#UNKNOWN#");
    }

    std::vector<LTSTransitionAssignment> determine_program_state_change(std::vector<LTSTransitionAssignment>& statements)
    {
        std::vector<LTSTransitionAssignment> state_change;
        std::unordered_map<std::string, std::shared_ptr<Expression>> state;
        for (const auto& [lhs, rhs] : statements)
        {
            // Check whether all the variables are tracked in the state
            // set a=<expr> if a is not tracked
            // add a=a' for each untracked variable in RHS
            if (state.find(lhs) == state.end())
            {
                state[lhs] = Expression::create_variable(lhs+"@");
            }

            for (const auto var : rhs->get_variable_names())
            {
                if (state.find(var) == state.end())
                {
                    state[var] = Expression::create_variable(var+"@");
                }
            }

            // Evaluate RHS over current program state and update program state after assignment to LHS
            auto state_rhs = rhs->copy();
            for (const auto var : state_rhs->get_variable_names())
            {
                state_rhs->substitute(var, state[var]->copy());
            }

            state[lhs] = state_rhs;
        }

        // Transition assignment modelling how the progrma state changes
        for (const auto& [lhs, rhs] : state)
        {
            // Remove @ from variable names in RHS
            for (const auto var : rhs->get_variable_names())
            {
                std::string new_name = var;
                new_name.pop_back();
                rhs->rename_variable(var, new_name);
            }

            // Ommit a = a
            if (Expression::create_variable(lhs) == rhs) continue;
            
            state_change.push_back(LTSTransitionAssignment(lhs, rhs));
        }

        return state_change;
    }
};

#endif