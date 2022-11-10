#include "math_instructions.h"




km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::add_instruction::exec(const wall_e::vec<std::shared_ptr<dyn_value>>&) {
    return exec_result {
        .value = NULL,
        .next_inst = index { .offset = null_size_t_ptr - 1 /** should cause error */, .ptr = NULL }
    };
}
