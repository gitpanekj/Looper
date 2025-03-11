
#include <string.h>
#include "lts_constructor/llvm_ir_processor.hpp"
#include "lts_constructor/exceptions.hpp"
#include "lts_constructor/lts_construction.hpp"
#include "graphs/labeled_transition_system.hpp"
#include "graphs/lts_labels.hpp"

#include "llvm/ADT/StringRef.h"     // StringRef
#include "llvm/Support/SourceMgr.h" // SMDiagnostic
#include "llvm/IRReader/IRReader.h" // parseIRFile

using LTS = graphs::LabeledTransitionSystem;

void LLVMIRProcessor::loadModule(const std::string &filename)
{
    // Try to parse IR file
    llvm::SMDiagnostic err;
    std::unique_ptr<llvm::Module> _module_handle = parseIRFile(llvm::StringRef(filename), err, module_context);

    // failed to parse file with IR
    if (_module_handle == nullptr)
    {
        throw InvalidFileException("Failed to parse provided LLVM IR bitcode.");
    }

    module_handle = std::move(_module_handle);

    for (llvm::Function &F : *module_handle)
    {
        functions.push_back(F.getName().str());
    }
}

llvm::Function *LLVMIRProcessor::getCFG(std::string &fname)
{
    return module_handle->getFunction(llvm::StringRef(fname));
}

LTS LLVMIRProcessor::getLTS(std::string &fname)
{
    // Check that module is loaded
    if (module_handle == nullptr)
    {
        throw ModuleNotLoadedException("Trying to get LTS without loaded module.");
    }
    // Try to get function CFG from module
    llvm::Function *cfg = module_handle->getFunction(llvm::StringRef(fname));
    if (!cfg)
    {
        throw InvalidFunctionNameException("Provided function name is not present in the loaded module.");
    }

    return constructLTSFromCFG(cfg);
}