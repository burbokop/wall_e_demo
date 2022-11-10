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
    if(offset == null_size_t_ptr) force_eval_offset(ctx);
}

km2::interpreter_backend::instruction::index km2::interpreter_backend::instruction::index::next(execution_ctx *ctx) const {
    if(offset != null_size_t_ptr) {
        index i { .offset = offset + 1, .ptr = nullptr };
        i.eval_ptr(ctx);
        return i;
    } else {
        ctx->add_err("trying increment index but it is not evaluated");
        ctx->fault();
        return {};
    }
}

