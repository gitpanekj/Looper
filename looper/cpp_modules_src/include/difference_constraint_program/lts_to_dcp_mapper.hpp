#ifndef LTS_TO_DCP_H
#define LTS_TO_DCP_H

#include "difference_constraint_program/difference_constraint_program.hpp"
#include "labeled_transition_system/labeled_transition_system.hpp"

namespace difference_constraint_program {
    void lts_to_dcp_mapper(labeled_transition_system::LabeledTransitionSystem& lts, DifferenceConstraintProgram& dcp);
}
#endif
