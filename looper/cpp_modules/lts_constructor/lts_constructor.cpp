#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>

#include "lts_constructor/lts_constructor.hpp"
#include "graphs/labeled_transition_system.hpp"

#include "llvm/ADT/StringRef.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IRReader/IRReader.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"


#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/Scalar/DeadStoreElimination.h"

#include "llvm/Analysis/CFGPrinter.h"

using namespace llvm;

namespace {

  struct MyAnalysisResult {
    int result;
  };

  // Define the analysis itself
  struct MyAnalysis : AnalysisInfoMixin<MyAnalysis> {
      using Result = MyAnalysisResult;
      static llvm::AnalysisKey Key;
      Result run(Function &F, FunctionAnalysisManager &AM) {
          // Perform analysis on function F
          Result res;
          res.result = 10;
          return res;
      }
  };

  llvm::AnalysisKey MyAnalysis::Key;



  struct LTSConstructWrapper : public PassInfoMixin<LTSConstructWrapper> {
    // Run the pass on the Module and return whether it modified the Module.
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
      llvm::errs() << "===Running functionPrinter on function " << F.getName() << "===\n";
      // CFG traversal and LTS construction
      
      for ( BasicBlock& BB : F)
      {
        llvm::errs() << BB.getName().str() << "\n";
      }
      llvm::errs() << "======================================\n";

      
      auto &Result = FAM.getResult<MyAnalysis>(F);


      // Indicate that this pass did not modify the Module.
      return PreservedAnalyses::all();
    }
  };
}

// Register the pass with the New Pass Manager
PassPluginLibraryInfo getMyPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MyPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            // Register the Module Pass with the PassBuilder
            PB.registerPipelineParsingCallback(
              [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == "function_printer") {
                  FPM.addPass(LTSConstructWrapper());
                  return true;
                }
                return false;
              }
            );
          }};
}

// Export the pass for dynamic loading
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getMyPassPluginInfo();
}

static MyAnalysisResult createLTS(Function &F)
{
  LoopAnalysisManager LAM;
  FunctionAnalysisManager FAM;
  CGSCCAnalysisManager CGAM;
  ModuleAnalysisManager MAM;

  // register analysis
  FAM.registerPass([&] { return MyAnalysis(); });

  PassBuilder PB;
  PB.registerModuleAnalyses(MAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);


  ModulePassManager MPM;
  FunctionPassManager FPM;

  FPM.addPass(DSEPass());         // dse
  FPM.addPass(InstCombinePass()); // instcombine
  FPM.addPass(PromotePass());     // mem2reg
  FPM.addPass(CFGPrinterPass());
  FPM.addPass(LTSConstructWrapper());
    
    
  //MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
  FPM.run(F, FAM);

  auto &Result = FAM.getResult<MyAnalysis>(F);
  return Result;
}



int LLVM_Module::load_module(const std::string& _filename)
{
    if (_filename.empty()) {
        std::cerr << "Filename is empty.\n";
        return -1; // Return an error code if the filename is empty
    }

    StringRef filename (_filename);
    SMDiagnostic Err;

    auto tempM = parseIRFile(filename, Err, Context);

    if (tempM == nullptr)
    {
        std::cerr << "Error\n";
        std::cerr << Err.getMessage().str() << std::endl;
        return -1;
    }

    M = std::move(tempM);

    functions.clear();
    for ( Function& F : *M )
    {
        functions.push_back(F.getName().str());
    }

    return 0;
}

LLVM_Module::~LLVM_Module(){
  std::cout << "Destroying LLVM_Module instance." << std::endl;
  M.release();
}

const std::vector<std::string> LLVM_Module::get_functions() { return functions; }

graphs::LabeledTransitionSystem<std::string, std::string> LLVM_Module::get_lts(const std::string& name){
  StringRef funcname(name);
  Function *F = M->getFunction(name);

  // TODO: place for building LTS

  return graphs::LabeledTransitionSystem<std::string, std::string>();
}