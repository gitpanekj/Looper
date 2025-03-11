#ifndef LTS_TO_DCP_H
#define LTS_TO_DCP_H

#include "graphs/difference_constraint_program.hpp"
#include "graphs/labeled_transition_system.hpp"

namespace graphs {
    void lts_to_dcp_mapper(LabeledTransitionSystem& lts, DifferenceConstraintProgram& dcp);
}
#endif
