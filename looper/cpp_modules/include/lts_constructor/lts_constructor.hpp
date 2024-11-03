#ifndef LTS_CONSTRUCTOR_H
#define LTS_CONSTRUCTOR_H
#include <string>
#include <vector>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "graphs/labeled_transition_system.hpp"

class LLVM_Module
{
private:
    std::unique_ptr<llvm::Module> M = nullptr;
    std::vector<std::string> functions;
    llvm::LLVMContext Context;

public:
    int load_module(const std::string &_functions);
    const std::vector<std::string> get_functions();
    graphs::LabeledTransitionSystem<std::string, std::string> get_lts(const std::string &name);

    ~LLVM_Module();
};

#endif