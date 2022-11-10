#include "call_instruction.h"

#include "execution_ctx.h"
#include "dyn_value.h"

km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::call_instruction::exec(
        const wall_e::vec<std::shared_ptr<dyn_value>>& args
        ) {
    if(m_f->is_native()) {
        return {
            .value = m_f->native()(ctx(), m_args),
            .next_inst = next_index()
        };
    } else {
        ctx()->stack_push(dyn_value::from_instruction_index(next_index()));
        ctx()->stack_push(m_args);
        return {
            .value = nullptr,
            .next_inst = m_f->entry(ctx())
        };
    }
}

km2::interpreter_backend::instruction::exec_result km2::interpreter_backend::ret_instruction::exec(
        const wall_e::vec<dyn_value::ptr>& vals
        ) {
    if(const auto& pop = ctx()->stack_pop(m_args_count + 1)) {
        if(pop->is_instruction_index()) {
            return {
                .value = vals.front_opt().value_or(nullptr),
                .next_inst = pop->instruction_index()
            };
        } else {
            ctx()->add_err("trying to return but top value is not a pointer");
            ctx()->fault();
        }
    }
    return {};
}

std::ostream &km2::interpreter_backend::call_instruction::print(std::ostream &stream) const {
    if(m_dbg_name.empty()) {
        return m_f->print(stream << "call ") << " args: " << m_args << " -> " << m_dbg_ret_ptr << "_s";
    } else {
        return m_f->print(stream << "call ") << " (" << m_dbg_name << ") args: " << m_args << " -> " << m_dbg_ret_ptr << "_s";
    }
}

std::ostream &km2::interpreter_backend::ret_instruction::print(std::ostream &stream) const {
    if(m_args_count > 0) {
        return stream << "ret " << (m_args_count + 1);
    } else {
        return stream << "ret";
    }
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

