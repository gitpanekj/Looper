/**
 * @file cfg_utils.cpp
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * @brief 
 * @version 0.1
 * @date 2024-12-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <set>
#include <vector>
#include <tuple>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "lts_constructor/cfg_utils.hpp"




bool isVisited(std::set<llvm::BasicBlock *> &visited_blocks, llvm::BasicBlock *block)
{
  return visited_blocks.count(block) > 0;
}

bool isBranching(llvm::BasicBlock *block)
{
  llvm::Instruction *terminator = block->getTerminator();
  return terminator->getNumSuccessors() > 1;
}

bool isTerminating(llvm::BasicBlock *block)
{
  llvm::Instruction *terminator = block->getTerminator();
  return llvm::isa<llvm::ReturnInst>(terminator);
}

bool isJoin(llvm::BasicBlock *block)
{
  return llvm::pred_size(block) > 1 && (!isTerminating(block));
}

bool isIntermediary(llvm::BasicBlock *block)
{
  llvm::Instruction *terminator = block->getTerminator();
  int pred = llvm::pred_size(block);
  int succ = terminator->getNumSuccessors();
  return (pred == 1 || pred == 0) && (succ == 1);
}

BasicBlockType getBasicBlockType(llvm::BasicBlock *block)
{
  BasicBlockType type;
  if (isBranching(block)) {type.set(BasicBlockType::Property::Branching);}
  if (isTerminating(block)) {type.set(BasicBlockType::Property::Terminating);}
  if (isJoin(block)) {type.set(BasicBlockType::Property::Join);}
  if (isIntermediary(block)) {type.set(BasicBlockType::Property::Intermediary);}

  return type;
}





std::string getFunctionName(llvm::Function *function_handle)
{
  std::string name = std::string(function_handle->getName());
  return name;
}


std::string getFunctionReturnType(llvm::Function *function_handle)
{
  std::string returnType;
  llvm::raw_string_ostream type_stream(returnType);
  function_handle->getReturnType()->print(type_stream);
  return returnType;
}

std::vector<std::pair<std::string, std::string>> getFunctionParameters(llvm::Function *function_handle)
{
  std::string return_type;
  llvm::raw_string_ostream type_stream(return_type);
  std::vector<std::pair<std::string, std::string>> parameters;

  for (llvm::Argument &Arg : function_handle->args())
  {
    Arg.getType()->print(type_stream);
    parameters.push_back({std::string(Arg.getName()), type_stream.str()});
    return_type.clear();
  }

  return parameters;
}



std::ostream& operator<< (std::ostream& os, const BasicBlockType& type) {
    os << " Type: ";
    if (type.is(BasicBlockType::Property::Terminating)){os << "Terminating ";}
    if (type.is(BasicBlockType::Property::Branching)){os << "Branching ";}
    if (type.is(BasicBlockType::Property::Join)){os << "Join ";}
    if (type.is(BasicBlockType::Property::Intermediary)){os << "Intermediary";}
    return os;
}