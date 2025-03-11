#ifndef LTS_CONSTRUCTION_HPP
#define LTS_CONSTRUCTION_HPP

#include <memory>
#include "llvm/IR/Function.h" // Function
#include "graphs/labeled_transition_system.hpp" // LabeledTransitionSystem

graphs::LabeledTransitionSystem constructLTSFromCFG(llvm::Function *cfg);

#endif