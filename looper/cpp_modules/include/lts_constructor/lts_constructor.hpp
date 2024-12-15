#ifndef LTS_CONSTRUCTOR_H
#define LTS_CONSTRUCTOR_H
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <tuple>
#include <set>

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/BasicBlock.h"

#include "graphs/labeled_transition_system.hpp"
#include "lts_constructor/cfg_utils.hpp"
#include "lts_constructor/instruction_interpreter.hpp"

using LTS = graphs::LabeledTransitionSystem<std::string, std::string>;

class LTSConstructor
{
private:
    std::unique_ptr<llvm::Module> module_handle = nullptr; // loaded module
    llvm::LLVMContext module_context;                 // module context
    std::vector<std::string> module_functions;        // module functions 


    // LTS construction
    InstructionInterpreter interpret;
    LTS* lts;
    llvm::BasicBlock *block_to_process;
    int previous_block_id;
    std::set<llvm::BasicBlock *> visited_blocks;
    std::unordered_map<llvm::BasicBlock *, int> basic_block_id;
    std::stack<std::pair<int, llvm::BasicBlock *>> dfs_stack;
    
    // construction utilities
    void bindLTS(LTS *_lts){lts = _lts;};
    void processFunctionSignature(llvm::Function* function_handle);
    BasicBlockType processBasicBlock();
    

public:
    void loadModule(const std::string &_functions);
    const std::vector<std::string> getFunctions() { return module_functions; };
    LTS getLTS(const std::string &name);
    ~LTSConstructor(){module_handle.release();}
};

#endif