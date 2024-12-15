/**
 * @file lts_constructor.cpp
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * @brief
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "lts_constructor/lts_constructor.hpp"
#include "graphs/labeled_transition_system.hpp"
#include "lts_constructor/cfg_utils.hpp"
#include "lts_constructor/exceptions.hpp"

#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/CFG.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"

#include <iostream>
#include <memory>
#include <unordered_map>
#include <set>
#include <stack>
#include <exception>

using namespace llvm;

/**
 * @brief Parse .bc file and load it to llvm module.
 *
 * @param _filename .bc file with llvm IR
 * @return int 0 on success, otherwise 1
 */
void LTSConstructor::loadModule(const std::string &_filename)
{
  // Empty file with llvm IR
  if (_filename.empty())
  {
    throw InvalidFileException("Provided file does not contains LLVM IR bitcode.");
  }

  // Try to parse IR file
  StringRef filename(_filename);
  SMDiagnostic err;
  auto temp = parseIRFile(filename, err, module_context);

  // failed to parse file with IR
  if (temp == nullptr)
  {
    throw InvalidFileException("Failed to parse provided LLVM IR bitcode.");
  }

  // Move LLVM IR in memmory representation handle to LTSConstructor module handle
  module_handle = std::move(temp);

  for (Function &F : *module_handle)
  {
    module_functions.push_back(F.getName().str());
  }
}

/**
 * @brief Construct LTS for a function in loaded module with given name.
 *
 * Module must be loaded first.
 *
 * @param name
 * @return LTS
 */
LTS LTSConstructor::getLTS(const std::string &name)
{

  std::cout << "Function: " + name << std::endl;
  // Check that module is loaded
  if (module_handle == nullptr)
  {
    throw ModuleNotLoadedException("Trying to get LTS without loaded module.");
  }

  // Try to get function CFG from module
  StringRef funcname(name);
  Function *function_handle = module_handle->getFunction(name);
  if (!function_handle)
  {
    throw InvalidFunctionNameException("Provided function name is not present in the loaded module.");
  }

  /*** LTS construction  ***/
  interpret.clearInterpret();
  LTS _lts;
  bindLTS(&_lts);
  lts->add_start_location("start location"); // start location l_s
  lts->add_end_location("exit location");    // exit location l_e
  processFunctionSignature(function_handle); // function signature

  // NOTE: as a result of performed LLVM passes a function may be left without any basic blocks
  // in that case, empty LTS is returned
  if (function_handle->empty())
  {
    lts->add_edge(lts->add_start_location(), lts->add_end_location());
    return _lts;
  }

  // Traverse CFG
  dfs_stack.push({lts->startLocation(), &function_handle->getEntryBlock()});
  while (!dfs_stack.empty())
  {

    std::pair<int, llvm::BasicBlock *> from_to = dfs_stack.top();
    dfs_stack.pop();
    previous_block_id = std::get<0>(from_to);
    block_to_process = std::get<1>(from_to);

    
    BasicBlockType basic_block_type = processBasicBlock();

    bool basic_block_is_visited = isVisited(visited_blocks, block_to_process);

    // update visited block and push nodes to process into the stack
    if (!basic_block_is_visited)
    {
      visited_blocks.insert(block_to_process);
      int from = basic_block_type.is(BasicBlockType::Property::Intermediary) ? previous_block_id : basic_block_id[block_to_process]; // intermediary nodes are skipped
      for (int i = 0; i < block_to_process->getTerminator()->getNumSuccessors(); i++)
      {
        dfs_stack.push({from, block_to_process->getTerminator()->getSuccessor(i)});
      }
    }
  }

  std::cout << "===============================" << std::endl;

  return _lts;
}

/// Private methods
void LTSConstructor::processFunctionSignature(llvm::Function *function_handle)
{
  lts->add_name(getFunctionName(function_handle));
  lts->add_ret(getFunctionReturnType(function_handle));
  lts->add_parameters(getFunctionParameters(function_handle));
}

BasicBlockType LTSConstructor::processBasicBlock()
{
  std::cout << "Processing: " << block_to_process->getName().str() << std::endl;
  BasicBlockType basic_block_type = getBasicBlockType(block_to_process);
  bool basic_block_is_visited = isVisited(visited_blocks, block_to_process);

  std::cout << basic_block_type << std::endl;
  std::cout << "  Visited: " << basic_block_is_visited << std::endl;


  interpret.checkPendingCondition(block_to_process->getName().str());

  // LTS structure
  if (basic_block_is_visited)
  {
    int src = previous_block_id;                // retrive LTS node ID of previous basic block
    int dst = basic_block_id[block_to_process]; // retrive LTS node ID of visited basic block

    BlockStatements stmts = interpret.getBlockStatement();
    interpret.clearCache();
    lts->add_edge(src, dst, stmts.toStr());    // add edge between the two visited LTS locations
  }
  else
  {
      for (const auto &inst : *block_to_process)
      {
        try {
          interpret.parseInstruction(&inst);
        }
        catch (const UnknownInstruction& err)
        {
          // std::cerr << "    Unknown instruction" << std::endl;
        }
      }


    // produce LTS location only for branching, join and terminating basic blocks
    // for intermediary nodes, only instructions are processed
    if (!basic_block_type.is(BasicBlockType::Property::Intermediary))
    {
      int src = previous_block_id;                                           // retrive LTS node ID of previous basic block
      int dst = lts->add_location(std::string(block_to_process->getName())); // create new location in LTS
      basic_block_id[block_to_process] = dst;                                // assign current basic block ID of create LTS location

      BlockStatements stmts = interpret.getBlockStatement();
      interpret.clearCache();
      lts->add_edge(src, dst, stmts.toStr());                                               // add edge from previous LTS location to the new one
    }

    // add edge to LTS exit location for terminating basic block
    if (basic_block_type.is(BasicBlockType::Property::Terminating))
    {
      lts->add_edge(basic_block_id[block_to_process], lts->endLocation());
    }
  }

  return basic_block_type;
}