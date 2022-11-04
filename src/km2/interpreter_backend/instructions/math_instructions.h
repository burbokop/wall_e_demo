#ifndef MATH_INSTRUCTIONS_H
#define MATH_INSTRUCTIONS_H

#include "instruction.h"

namespace km2 {
namespace interpreter_backend {

class add_instruction : instruction {
public:
    add_instruction(execution_ctx* ctx) : instruction(ctx) {}

    // instruction interface
public:
    virtual exec_result exec(const wall_e::vec<dyn_value::ptr>&) override;
};

}
}

#endif // MATH_INSTRUCTIONS_H
