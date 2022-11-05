#include "math_instructions.h"




km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::add_instruction::exec(const wall_e::vec<dyn_value::ptr>&) {
    return exec_result {
        .value = NULL,
        .next_inst = index { .offset = index::null_offset - 1 /** should cause error */, .ptr = NULL }
    };
}
