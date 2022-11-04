#include "instruction.h"

#include "execution_ctx.h"

km2::interpreter_backend::instruction::index km2::interpreter_backend::instruction::next_index() const {
    return m_ctx->current_index().next(m_ctx);
}

void km2::interpreter_backend::instruction::index::eval_ptr(execution_ctx *ctx) { ptr = ctx->instructions()[offset]; }

void km2::interpreter_backend::instruction::index::force_eval_offset(execution_ctx *ctx) {
    if(ptr) {
        const auto& inss = ctx->instructions();
        for(std::size_t i = 0; i < inss.size(); ++i) {
            if(inss[i] == ptr) {
                offset = i;
                break;
            }
        }
    }
}

void km2::interpreter_backend::instruction::index::eval_offset(execution_ctx *ctx) {
    if(offset == null_offset) force_eval_offset(ctx);
}

km2::interpreter_backend::instruction::index km2::interpreter_backend::instruction::index::next(execution_ctx *ctx) const {
    if(offset != null_offset) {
        index i { .offset = offset + 1, .ptr = nullptr };
        i.eval_ptr(ctx);
        return i;
    } else {
        ctx->add_err("trying increment index but it is not evaluated");
        ctx->fault();
        return {};
    }
}

const wall_e::variant &km2::interpreter_backend::dyn_value::value(execution_ctx *ctx) const {
    if(m_stack_ptr != std::numeric_limits<std::size_t>::max()) {
        if(const auto& sf = ctx->call_stack_top()) {
            if(m_stack_ptr < sf->values.size()) {
                return sf->values[m_stack_ptr]->value(ctx);
            }
        }
        static const wall_e::variant r;
        return r;
    } else {
        return m_value;
    }
}
