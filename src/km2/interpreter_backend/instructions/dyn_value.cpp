#include "dyn_value.h"
#include "execution_ctx.h"

const wall_e::variant &km2::interpreter_backend::dyn_value::value(execution_ctx *ctx, const wall_e::opt<size_t_ptr>& first_stack_ptr) const {
    static const wall_e::variant default_res;
    if(is_stack_ptr()) {
        const auto pointed_value = ctx->try_dereference(stack_ptr());
        if(pointed_value->points_to(*first_stack_ptr)) {
            std::stringstream ss;
            ss << "ptr dereferencing loop detected in dyn val: " << this;
            ctx->add_err(ss.str());
            ctx->fault();
            return default_res;
        } else {
            return pointed_value->value(ctx, first_stack_ptr);
        }
        return default_res;
    } else if(is_var()) {
        return var();
    } else {
        return default_res;
    }
}

