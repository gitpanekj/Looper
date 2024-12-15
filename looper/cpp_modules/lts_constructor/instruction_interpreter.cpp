#include <string>
#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "lts_constructor/instruction_interpreter.hpp"
#include "lts_constructor/exceptions.hpp"
#include "expression/expression.hpp"
#include <iostream>
#include <memory>

void InstructionInterpreter::parseInstruction(const llvm::Instruction *inst)
{
    switch (inst->getOpcode())
    {
    case llvm::Instruction::Store:
        interpretStore(inst);
        break;
    case llvm::Instruction::Load:
        interpretLoad(inst);
        break;
    case llvm::Instruction::Add:
        interpretAdd(inst);
        break;
    case llvm::Instruction::Sub:
        interpretSub(inst);
        break;
    case llvm::Instruction::Mul:
        interpretMul(inst);
        break;
    case llvm::Instruction::ICmp:
        interpretCmp(inst);
        break;
    case llvm::Instruction::Br:
        interpretConditionalJump(inst);
        break;
    default:
        throw UnknownInstruction("Unknown instruction");
    }
}

std::shared_ptr<expression::Term> InstructionInterpreter::getOperand(llvm::Value *operand)
{
    
    // named variable
    if (operand->hasName())
    {
        std::string var_name = operand->getName().str();
        auto it = expression_cache.find(var_name);
        if (it != expression_cache.end())
        {
            return it->second;
        }

        return std::make_shared<expression::AccessPath>(var_name);
    }

    // temporary variable
    if (llvm::isa<llvm::Instruction>(operand))
    {
        std::string temp_var_name = operand->getNameOrAsOperand();
        auto it = expression_cache.find(temp_var_name);
        if (it != expression_cache.end())
        {
            return it->second;
        }

        return std::make_shared<expression::AccessPath>(temp_var_name);
    }

    // constant
    if (llvm::isa<llvm::ConstantInt>(operand))
    {
        llvm::ConstantInt *const_int = (llvm::ConstantInt *)operand;
        return std::make_shared<expression::IntConstant>(const_int->getValue().getSExtValue());
    }

    // unknow
    return std::make_shared<expression::AccessPath>("#UNKNOWN#");
}

void InstructionInterpreter::interpretAdd(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 =    getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 =    getOperand(inst->getOperand(1));
    std::string lhs = inst->getName().str();
    

    if (auto c = std::dynamic_pointer_cast<expression::IntConstant>(op2))
    {
        if (c->getValue() < 0)
        {
            c->negate();
            auto sub = std::make_shared<expression::Sub>(op1, op2);
            expression_cache[lhs] = sub;
        }
        else
        {
          auto add = std::make_shared<expression::Add>(op1, op2);
            expression_cache[lhs] = add;  
        }
    }
    else {
        auto add = std::make_shared<expression::Add>(op1, op2);
        expression_cache[lhs] = add;
    }
}

void InstructionInterpreter::interpretSub(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 =    getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 =    getOperand(inst->getOperand(1));
    std::string lhs = inst->getName().str();
    auto sub = std::make_shared<expression::Sub>(op1, op2);
    expression_cache[lhs] = sub;
}

void InstructionInterpreter::interpretMul(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 =    getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 =    getOperand(inst->getOperand(1));
    std::string lhs = inst->getName().str();
    auto mul = std::make_shared<expression::Mul>(op1, op2);
    expression_cache[lhs] = mul;
}

void InstructionInterpreter::interpretLoad(const llvm::Instruction *inst)
{
    std::string lhs = inst->getNameOrAsOperand();
    std::shared_ptr<expression::Term> load = getOperand(inst->getOperand(0));
    expression_cache[lhs] = load;
}

void InstructionInterpreter::interpretStore(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::AccessPath> target = std::static_pointer_cast<expression::AccessPath>(getOperand(inst->getOperand(1)));
    std::shared_ptr<expression::Term> store = getOperand(inst->getOperand(0));
    std::cout << "Store: " << store->toStr() << std::endl;
    
    statement_batch.push_back(Assignment(target, store));
}

void InstructionInterpreter::interpretCmp(const llvm::Instruction *inst)
{
    llvm::ICmpInst *cmp = (llvm::ICmpInst *)inst;
    switch (cmp->getPredicate())
    {
    case llvm::ICmpInst::Predicate::ICMP_EQ:
        return interpretEqual(inst);
        break;
    case llvm::ICmpInst::Predicate::ICMP_NE:
        return interpretNotEqual(inst);
        break;
    case llvm::ICmpInst::Predicate::ICMP_SGE:
    case llvm::ICmpInst::Predicate::ICMP_UGE:
        return interpretGreaterEqual(inst);
        break;
    case llvm::ICmpInst::Predicate::ICMP_UGT:
    case llvm::ICmpInst::Predicate::ICMP_SGT:
        return interpretGreater(inst);
        break;
    case llvm::ICmpInst::Predicate::ICMP_SLE:
    case llvm::ICmpInst::Predicate::ICMP_ULE:
        return interpretLessEqual(inst);
        break;
    case llvm::ICmpInst::Predicate::ICMP_ULT:
    case llvm::ICmpInst::Predicate::ICMP_SLT:
        return interpretLess(inst);
        break;
    default:
        throw UnknownInstruction("Unknown compare instruction");
    }
}

void InstructionInterpreter::interpretLess(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::Less>(op1, op2);   
}

void InstructionInterpreter::interpretLessEqual(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::LessOrEqual>(op1, op2); 
}

void InstructionInterpreter::interpretGreater(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::Greater>(op1, op2); 
}

void InstructionInterpreter::interpretGreaterEqual(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::GreaterOrEqual>(op1, op2); 
}

void InstructionInterpreter::interpretEqual(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::Equal>(op1, op2); 
}

void InstructionInterpreter::interpretNotEqual(const llvm::Instruction *inst)
{
    std::shared_ptr<expression::Term> op1 = getOperand(inst->getOperand(0));
    std::shared_ptr<expression::Term> op2 = getOperand(inst->getOperand(1));
    std::string result = inst->getName().str();
    predicate_cache[result] = std::make_shared<expression::NotEqual>(op1, op2);  
}

void InstructionInterpreter::interpretConditionalJump(const llvm::Instruction *inst)
{
    llvm::BranchInst *br = (llvm::BranchInst *)inst;
    if (br->isUnconditional()) throw UnknownInstruction("Unconditional jump");

    std::shared_ptr<expression::Predicate> condition = predicate_cache.find(br->getCondition()->getName().str())->second;
    

    std::string true_branch = br->getSuccessor(0)->getName().str();
    auto false_branch = br->getSuccessor(1)->getName().str();

    condtion_to_label_mapping[true_branch] = condition;
    condtion_to_label_mapping[false_branch] = condition->negate();
}
