#ifndef LLVM_IR_PROCESSOR
#define LLVM_IR_PROCESSOR

#include <memory> // unique_ptr
#include <vector> // vector
#include <string> // string

#include "llvm/IR/Module.h"      // Module
#include "llvm/IR/LLVMContext.h" // LLVMContext
#include "llvm/IR/Function.h"    // Function

#include "graphs/labeled_transition_system.hpp" // LabeledTransitionSystem
#include "graphs/lts_labels.hpp"

using LTS = graphs::LabeledTransitionSystem;


/**
 * @brief A class responsible for loading LLVM module and mainly one of representations of contained functions.
 *
 * A class responsible for loading LLVM module and
 * mainly one of representations of contained functions along
 * with both module and functions' metadata.
 * 
 * Information about the module and module metadata include
 * 1. contained functions
 * 2. TODO: call graph
 * 3. FUTURE: other required metadata.
 * 
 * For a single function folowing metadata can be obtained:
 * 1. function signature,
 * 2. FUTURE: other required metadata, i.e. call dependencies
 * and a function itself can be obtained in several representations including
 * 1. Control Flow Graph (CFG),
 * 2. Labeled Transition System (LTS),
 * 3. FUTURE: other required representation
 * 
 * 
 * LLVM module is a single unit of code in the LLVM IR containing entities
 * that are part of a compiled program, most importantly functions with according CFG).
 * LLVM module may be a result of compilation and linkage of several source files.
 *
 */
class LLVMIRProcessor
{
private:
    // Attributes
    std::unique_ptr<llvm::Module> module_handle = nullptr; // module
    llvm::LLVMContext module_context;                      // module context - TODO: explain why it is required.
    // TODO: consider whether to track functions or symbols in general
    // TODO: for now only function names are tracked, this should be replaced with
    //       dict containing metadata for a function/symbol - lazy evaluation
    std::vector<std::string> functions;                    // contained functions

    // Methods
    // ...

public:

    // Loading module
    void loadModule(const std::string &filename);

    // Acess to module information and metadata
    const std::vector<std::string>& getFunctions() { return functions; }

    // Access to function representation and metada
    llvm::Function* getCFG(std::string &fname);
    LTS getLTS(std::string &fname);


    // Module Lifecycle
    ~LLVMIRProcessor(){ module_handle.release(); }
};

#endif