#ifndef LTS_CONSTRUCTION_HPP
#define LTS_CONSTRUCTION_HPP

#include <memory>
#include "llvm/IR/Function.h" // Function
#include "labeled_transition_system/labeled_transition_system.hpp" // LabeledTransitionSystem

labeled_transition_system::LabeledTransitionSystem constructLTSFromCFG(llvm::Function *cfg);

#endif