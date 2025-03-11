#ifndef LTS_LABEL_BUILDER
#define LTS_LABEL_BUILDER

#include "llvm_ir_interpreter_base.hpp"
#include "graphs/lts_labels.hpp"
#include "LTS_execution_context.hpp"

LLVM_IR_INTERPRETER_INSTRUCTION_LIST(LTSLabelBuilder, TransitionExecutionContext)
    // Arithmetic
    ADD_INSTRUCTION(Add)
    // ADD_INSTRUCTION(FAdd, LabelType);
    ADD_INSTRUCTION(Sub)
    // ADD_INSTRUCTION(FSub, LabelType);
    ADD_INSTRUCTION(Mul)
    // ADD_INSTRUCTION(FMu,  LabelType);
    // ADD_INSTRUCTION(UDiv, LabelType);
    // ADD_INSTRUCTION(SDiv, LabelType);
    // ADD_INSTRUCTION(FDiv, LabelType);
    // ADD_INSTRUCTION(URem, LabelType);
    // ADD_INSTRUCTION(SRem, LabelType);
    // ADD_INSTRUCTION(FRem, LabelType);

    // Bitwise
    // ADD_INSTRUCTION(Shl, LabelType);
    // ADD_INSTRUCTION(LSh, LabelType);
    // ADD_INSTRUCTION(ASh, LabelType);
    // ADD_INSTRUCTION(And, LabelType);
    // ADD_INSTRUCTION(Or,  LabelType);
    // ADD_INSTRUCTION(Xor, LabelType);

    // Compare
    ADD_INSTRUCTION(ICmp)
    ADD_INSTRUCTION(Br)
    // ADD_INSTRUCTION(FCmp, LabelType);

    // Function call
    //ADD_INSTRUCTION(Call)

    // Memmory
    // ADD_INSTRUCTION(Alloc, LabelType);
    ADD_INSTRUCTION(Load)
    ADD_INSTRUCTION(Store)
LLVM_IR_INTERPRETER_INSTRUCTION_LIST_END

#endif