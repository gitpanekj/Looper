#include "lts_constructor/LLVMIRInterpreter/lts_label_builder.hpp"
#include "lts_constructor/LLVMIRInterpreter/LTS_execution_context.hpp"
#include "expression/expression.hpp"
#include "expression/predicate.hpp"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"

LLVM_IR_INTERPRETER_INSTRUCTION_HANDLERS(LTSLabelBuilder, TransitionExecutionContext)
    // Arithmetic
    INSTRUCTION_HANDLER(Add){
        std::shared_ptr<Expression> op1 = ctx.get_operand( inst->getOperand(0));
        std::shared_ptr<Expression> op2 = ctx.get_operand( inst->getOperand(1));
        std::string store_access_path = inst->getName().str();
        std::shared_ptr<Expression> add = Expression::create_addition(op1, op2);
        ctx.expression_cache[store_access_path] = add;
    }
    //IMPLEMENTS_INSTRUCTION(FAdd);
    INSTRUCTION_HANDLER(Sub){
        std::shared_ptr<Expression> op1 = ctx.get_operand( inst->getOperand(0));
        std::shared_ptr<Expression> op2 = ctx.get_operand( inst->getOperand(1));
        std::string store_access_path = inst->getName().str();
        std::shared_ptr<Expression> sub = Expression::create_subtraction(op1, op2);
        ctx.expression_cache[store_access_path] = sub;
    }
    //IMPLEMENTS_INSTRUCTION(FSub);
    INSTRUCTION_HANDLER(Mul){
        std::shared_ptr<Expression> op1 = ctx.get_operand( inst->getOperand(0));
        std::shared_ptr<Expression> op2 = ctx.get_operand( inst->getOperand(1));
        std::string store_access_path = inst->getName().str();
        std::shared_ptr<Expression> mul = Expression::create_multiplication(op1, op2);
        ctx.expression_cache[store_access_path] = mul;
    }
    //IMPLEMENTS_INSTRUCTION(FMul);
    //IMPLEMENTS_INSTRUCTION(UDiv);
    //IMPLEMENTS_INSTRUCTION(SDiv);
    //IMPLEMENTS_INSTRUCTION(FDiv);
    //IMPLEMENTS_INSTRUCTION(URem);
    //IMPLEMENTS_INSTRUCTION(SRem);
    //IMPLEMENTS_INSTRUCTION(FRem);

    // Bitwise
    //IMPLEMENTS_INSTRUCTION(Shl);
    //IMPLEMENTS_INSTRUCTION(LSh);
    //IMPLEMENTS_INSTRUCTION(ASh);
    //IMPLEMENTS_INSTRUCTION(And);
    //IMPLEMENTS_INSTRUCTION(Or,);
    //IMPLEMENTS_INSTRUCTION(Xor);

    // Compare
    INSTRUCTION_HANDLER(ICmp){
        std::shared_ptr<Expression> op1 = ctx.get_operand( inst->getOperand(0));
        std::shared_ptr<Expression> op2 = ctx.get_operand( inst->getOperand(1));
        std::string store_access_path = inst->getName().str();

        switch (((llvm::ICmpInst *) inst)->getPredicate())
        {
            case llvm::ICmpInst::Predicate::ICMP_EQ:
                ctx.predicate_cache[store_access_path] = std::make_shared<Equal>(op1, op2);
                break;
            case llvm::ICmpInst::Predicate::ICMP_NE:
                ctx.predicate_cache[store_access_path] = std::make_shared<NotEqual>(op1, op2);
                break;
            case llvm::ICmpInst::Predicate::ICMP_SGE:
            case llvm::ICmpInst::Predicate::ICMP_UGE:
                ctx.predicate_cache[store_access_path] = std::make_shared<GreaterOrEqual>(op1, op2);
                break;
            case llvm::ICmpInst::Predicate::ICMP_UGT:
            case llvm::ICmpInst::Predicate::ICMP_SGT:
                ctx.predicate_cache[store_access_path] = std::make_shared<Greater>(op1, op2);
                break;
            case llvm::ICmpInst::Predicate::ICMP_SLE:
            case llvm::ICmpInst::Predicate::ICMP_ULE:
                ctx.predicate_cache[store_access_path] = std::make_shared<LessOrEqual>(op1, op2);
                break;
            case llvm::ICmpInst::Predicate::ICMP_ULT:
            case llvm::ICmpInst::Predicate::ICMP_SLT:
                ctx.predicate_cache[store_access_path] = std::make_shared<Less>(op1, op2);
                break;
        }
    }

    INSTRUCTION_HANDLER(Br){
        llvm::BranchInst *br = (llvm::BranchInst *)inst;
        if (br->isUnconditional()){
            // TODO: logging
            return;
        }
    
        std::shared_ptr<Predicate> condition_predicate;
        // if the condition contains invalidated variales produce stub conditinos
        // to let the analysis flow
        std::string condition = br->getCondition()->getNameOrAsOperand();
        if (ctx.invalidated_variables.find(condition) != ctx.invalidated_variables.end() ||
            ctx.predicate_cache.find(condition) == ctx.predicate_cache.end()) // this should not be possible
        {
            condition_predicate = std::make_shared<Equal>(Expression::create_variable("#INVALID"), Expression::create_variable("#INVALID"));
        }
        else
        {
            condition_predicate = ctx.predicate_cache.find(condition)->second;
        }

        std::string true_branch = br->getSuccessor(0)->getName().str();
        std::string false_branch = br->getSuccessor(1)->getName().str();
    
        ctx.block_name_to_predicate[true_branch] = {condition_predicate, true};
        ctx.block_name_to_predicate[false_branch] = {condition_predicate->negate(), false};
    }
    //IMPLEMENTS_INSTRUCTION(FCmp);

    // Function call
    // INSTRUCTION_HANDLER(Call){}

    // Memmory
    INSTRUCTION_HANDLER(Alloca){
        // NOTE: this instruciton has not effect on the transition labeld
        // If this handler was not added %var = alloca <type>, ... would raise exception
        // and %var would be invalidated in the program.
    }
    INSTRUCTION_HANDLER(Load){
        std::string load_access_path = inst->getNameOrAsOperand();
        if (ctx.invalidated_variables.find(load_access_path) != ctx.invalidated_variables.end()){
            throw InvalidatedValue("Invalid operand");
        }

        try{
            std::shared_ptr<Expression> loaded_value = ctx.get_operand(inst->getOperand(0));
            ctx.expression_cache[load_access_path] = loaded_value;
        }
        catch (InvalidatedValue&e){ // Invalidate load access path
            ctx.invalidated_variables.insert(load_access_path);
        }
    }
    INSTRUCTION_HANDLER(Store){
        std::string store_access_path = inst->getOperand(1)->getNameOrAsOperand();
        if (ctx.invalidated_variables.find(store_access_path) != ctx.invalidated_variables.end()){
            throw InvalidatedValue("Invalid operand");
        }

        try {
            std::shared_ptr<Expression> stored_value = ctx.get_operand(inst->getOperand(0));
            ctx.statement_batch.push_back(LTSTransitionAssignment(store_access_path, stored_value));
            //ctx.expression_cache[store_access_path] = stored_value;
        }
        catch (InvalidatedValue&e){ // Invalidate store acess path
            ctx.invalidated_variables.insert(store_access_path);
        }
    }

    INSTRUCTION_HANDLER(PHI){
        if (ctx.previous_basic_block == nullptr){
            throw InvalidatedValue("Invalid operand");
        }

        auto *phi = llvm::dyn_cast<llvm::PHINode>(inst);
        for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
            llvm::Value *val = phi->getIncomingValue(i);
            llvm::BasicBlock *bb = phi->getIncomingBlock(i);
            std::string val_name = val->getNameOrAsOperand();
            std::string store_access_path = inst->getNameOrAsOperand();
            
            if (bb == ctx.previous_basic_block)
            {
                if (auto *CI = llvm::dyn_cast<llvm::ConstantInt>(val)) {
                    if (CI->getType()->isIntegerTy(1)) {
                        if (CI->isOne()) {
                            ctx.predicate_cache[store_access_path] = std::make_shared<Equal>(Expression::create_constant(0),Expression::create_constant(0));
                        } else if (CI->isZero()){
                            ctx.predicate_cache[store_access_path] = std::make_shared<NotEqual>(Expression::create_constant(0),Expression::create_constant(0));
                        } else {
                            throw InvalidatedValue("Invalid operand");
                        }
                    }
                    break;
                }

                auto it = ctx.predicate_cache.find(val_name);
                if (it == ctx.predicate_cache.end()){
                    throw InvalidatedValue("Invalid operand");
                }
    
                ctx.predicate_cache[store_access_path] = it->second;
                break;
            }
        }
    }
LLVM_IR_INTERPRETER_INSTRUCTION_HANDLERS_END