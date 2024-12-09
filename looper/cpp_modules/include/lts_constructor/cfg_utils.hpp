#ifndef CFG_UTILS
#define CFG_UTILS


#include <set>
#include <string>
#include <iostream>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"

struct BasicBlockType
{
  enum Property
  {
    Terminating  = 1 << 0, // contains ret instruction and has no successors
    Branching    = 1 << 1, // has 2 or more direct successors and contains br or switch instruction
    Join         = 1 << 2, // has 2 or more direct predecessors (contains phi instruction)
    Intermediary = 1 << 3, // has 1 direct predecessor and 1 direct successor
  };

  unsigned int properties;

  BasicBlockType() : properties(0) {};
  bool is(Property p) const {return properties & p;}
  void set(Property p) { properties = properties | p;}
  void unset(Property p) {properties = properties & (~p);}
};
std::ostream& operator<< (std::ostream& os, const BasicBlockType& type);


// CFG traversal utils
bool isVisited(std::set<llvm::BasicBlock *>&, llvm::BasicBlock *); //!< check whether CFG basic block was already visited
bool isBranching(llvm::BasicBlock *);                              //!< check whether CFG basic block has 2 or more outgoing edges
bool isTerminating(llvm::BasicBlock *);                            //!< check whether CFG basic block contains ret instruction
bool isJoin(llvm::BasicBlock *);                                   //!< check whether CFG basic block has 2 or more incoming edges
bool isIntermediary(llvm::BasicBlock *);                           //!< check whether CFG basic block has 0/1 predecessors and 1 successor
BasicBlockType getBasicBlockType(llvm::BasicBlock *);              //!< get basic block properties: branching, join, terminating, intermediary

// getters
std::string getFunctionName(llvm::Function *);    //!< return name of function pointed by provided function handle
std::string getFunctionReturnType(llvm::Function *);               //!< return return type of function pointed by provided function handle
std::vector<std::pair<std::string, std::string>> getFunctionParameters(llvm::Function *); //!< return function parameters of function pointed by provided function handle

#endif