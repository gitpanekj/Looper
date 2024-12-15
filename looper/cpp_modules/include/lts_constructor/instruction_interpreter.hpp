#ifndef INST_INT_HPP
#define INST_INT_HPP

#include <string>
#include <memory>
#include "llvm/IR/Value.h"
#include "llvm/IR/Instruction.h"
#include "expression/expression.hpp"
#include <unordered_map>

typedef struct Assignment
{
    std::shared_ptr<expression::AccessPath> lhs;
    std::shared_ptr<expression::Term> rhs;
    Assignment(std::shared_ptr<expression::AccessPath> _lhs, std::shared_ptr<expression::Term> _rhs) : lhs(_lhs), rhs(_rhs) {};
};

typedef struct Condition
{
    std::shared_ptr<expression::Predicate> condition;
    Condition(std::shared_ptr<expression::Predicate> _condition) : condition(_condition) {}
    Condition() { condition = nullptr; };
};

typedef struct BlockStatements
{
    Condition condtition;
    std::vector<Assignment> statements;
    BlockStatements(Condition cond, std::vector<Assignment> stms) : condtition(cond), statements(stms) {}
    BlockStatements(std::vector<Assignment> stms) : statements(stms) {}
    BlockStatements() {}

    std::string toStr()
    {
        std::string out;
        if (condtition.condition)
        {
            out += condtition.condition->toStr() + "\n";
        }

        for (const auto &it : statements)
        {
            out += it.lhs->toStr() + " = " + it.rhs->toStr() + "\n";
        }

        return out;
    };
};

class InstructionInterpreter
{
public:
    void parseInstruction(const llvm::Instruction *inst);
    void clearCache()
    {
        expression_cache.clear();
        predicate_cache.clear();
        statement_batch.clear();
    };
    void clearInterpret()
    {
        expression_cache.clear();
        predicate_cache.clear();
        condtion_to_label_mapping.clear();
        statement_batch.clear();
        pending_condition = Condition();
    }

    void checkPendingCondition(std::string basic_block_name)
    {
        auto it = condtion_to_label_mapping.find(basic_block_name);
        if (it != condtion_to_label_mapping.end())
        {
            pending_condition = Condition(it->second);
            condtion_to_label_mapping.erase(basic_block_name);
        }
    }

    BlockStatements getBlockStatement()
    {
        if (pending_condition.condition)
        {
            auto rv = BlockStatements(pending_condition, statement_batch);
            pending_condition = Condition();
            return rv;
        }
        else
        {
            return BlockStatements(statement_batch);
        }
    }

private:
    std::unordered_map<std::string, std::shared_ptr<expression::Term>> expression_cache;
    std::unordered_map<std::string, std::shared_ptr<expression::Predicate>> predicate_cache;
    std::unordered_map<std::string, std::shared_ptr<expression::Predicate>> condtion_to_label_mapping;

    std::vector<Assignment> statement_batch;
    Condition pending_condition;

    std::shared_ptr<expression::Term> getOperand(llvm::Value *operand);

    void interpretAdd(const llvm::Instruction *inst);
    void interpretSub(const llvm::Instruction *inst);
    void interpretMul(const llvm::Instruction *inst);
    void interpretLoad(const llvm::Instruction *inst);
    void interpretStore(const llvm::Instruction *inst);

    void interpretCmp(const llvm::Instruction *inst);
    void interpretLess(const llvm::Instruction *inst);
    void interpretLessEqual(const llvm::Instruction *inst);
    void interpretGreater(const llvm::Instruction *inst);
    void interpretGreaterEqual(const llvm::Instruction *inst);
    void interpretEqual(const llvm::Instruction *inst);
    void interpretNotEqual(const llvm::Instruction *inst);
    void interpretConditionalJump(const llvm::Instruction *inst);
};

#endif