#ifndef LLVM_IR_INTERPRETER_BASE_H
#define LLVM_IR_INTERPRETER_BASE_H

#include <string>
#include <exception>
#include <iostream>
#include "llvm/IR/Instruction.h"


// TODO: some instructions are missing - https://llvm.org/docs/LangRef.html#instruction-reference
    // Terminator
    // Vector
    // Aggerate
    // Memmory access
    // Conversion
    // Others
#define INSTRUCTIONS \
    X(Add)\
    X(FAdd)\
    X(Sub)\
    X(FSub)\
    X(Mul)\
    X(FMul)\
    X(UDiv)\
    X(SDiv)\
    X(FDiv)\
    X(URem)\
    X(SRem)\
    X(FRem)\
    X(Shl)\
    X(LShr)\
    X(AShr)\
    X(And)\
    X(Or)\
    X(Xor)\
    X(ICmp)\
    X(Br)\
    X(FCmp)\
    X(Call)\
    X(Alloca)\
    X(Load)\
    X(Store)

// Interpreter definition macros
#define LLVM_IR_INTERPRETER_INSTRUCTION_LIST(InterpreterName, _InterpretationContext) \
    namespace InterpreterName {             \
    using InterpretationContext = _InterpretationContext;\
    class LTSLabelBuilder : public LLVMIRInterpreterBase<InterpretationContext>{

#define ADD_INSTRUCTION(name)       \
    void interpret##name##Instruction(              \
    const llvm::Instruction *inst,                  \
    InterpretationContext &ctx) override; 

#define LLVM_IR_INTERPRETER_INSTRUCTION_LIST_END };}


// Interpreter instruction handler definition macros
#define LLVM_IR_INTERPRETER_INSTRUCTION_HANDLERS(InterpreterName, _InterpretationContext)\
    namespace InterpreterName {                 \
    using InterpretationContext = _InterpretationContext;    \
    using InterpreterClsName = InterpreterName;

#define INSTRUCTION_HANDLER(name)  \
    void InterpreterClsName::interpret##name##Instruction(  \
    const llvm::Instruction *inst,                          \
    InterpretationContext &ctx)

#define LLVM_IR_INTERPRETER_INSTRUCTION_HANDLERS_END }


using LLVMInstructionType = unsigned int;
class InstructionNotImplemented : public std::exception
{
private:
    std::string instruction_name;

public:
    InstructionNotImplemented(const char *iname)
        : instruction_name(iname) {}

    const char *what()
    {
        return ("Instruction " + instruction_name + " is not implemented").c_str();
    }
};



template <typename InterpretationContext>
class LLVMIRInterpreterBase
{
protected:
    #define X(name)                                    \
        virtual void interpret##name##Instruction(     \
            const llvm::Instruction *inst,             \
            InterpretationContext &ctx)                \
            {throw InstructionNotImplemented(#name);}
        INSTRUCTIONS
    #undef X
public:
    void interpretInstruction(const llvm::Instruction *inst, InterpretationContext &ctx){
        LLVMInstructionType type = inst->getOpcode();
        
        switch (type){
            #define X(name)                                     \
                case llvm::Instruction::name:                   \
                    interpret##name##Instruction(inst, ctx);    \
                    break;
                INSTRUCTIONS
            #undef X

            default:
                // TODO: logging
                break;
        }
    }
};

#endif