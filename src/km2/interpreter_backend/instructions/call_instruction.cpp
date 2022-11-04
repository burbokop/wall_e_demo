#include "call_instruction.h"

#include "execution_ctx.h"

km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::call_instruction::exec(
        const wall_e::vec<dyn_value::ptr>& args
        ) {
    if(m_f->is_native()) {
        return {
            .value = m_f->native()(ctx(), m_args),
            .next_inst = next_index()
        };
    } else {
        ctx()->call_stack_push({
            .ret = next_index(),
            .values = m_args
        });
        return {
            .value = nullptr,
            .next_inst = m_f->entry(ctx())
        };
    }
}

km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::ret_instruction::exec(
        const wall_e::vec<dyn_value::ptr>& vals
        ) {
    return {
        .value = vals.front_opt().value_or(nullptr),
        .next_inst = ctx()->call_stack_pop().value_or(execution_ctx::stack_frame {}).ret
    };
}

std::ostream &km2::interpreter_backend::call_instruction::print(std::ostream &stream) const {
    return m_f->print(stream << "call ");
}

std::ostream &km2::interpreter_backend::ret_instruction::print(std::ostream &stream) const {
    return stream << "ret";
}

const km2::interpreter_backend::instruction::index &km2::interpreter_backend::dyn_function::entry(execution_ctx *ctx) const {
    m_entry.eval_offset(ctx);
    return m_entry;
}

std::ostream &km2::interpreter_backend::dyn_function::print(std::ostream &stream) const {
    if(is_native()) {
        return stream << "{ native }";
    } else {
        return stream << "{ entry: " << m_entry << " }";
    }
}

