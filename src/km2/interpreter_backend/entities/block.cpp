#include "block.h"

#include "function.h"

km2::interpreter_backend::block::block(execution_ctx *ctx, const std::string &name, function *f, std::size_t stack_offset)
    : m_ctx(ctx),
      m_name(name),
      m_f(f),
      m_stack_offset(stack_offset) { f->m_blocks.push_back(this); }

std::string km2::interpreter_backend::block::name() const {
    return m_name;
}

km2::backend::function *km2::interpreter_backend::block::func() const {
    return m_f;
}

std::ostream &km2::interpreter_backend::block::print(std::ostream &stream) const {
    return stream << "{ name: " << m_name
                  << ", instructions: " << m_instructions
                  << " }";
}
