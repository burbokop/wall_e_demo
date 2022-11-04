#include "block.h"

#include "function.h"

km2::interpreter_backend::block::block(execution_ctx *ctx, const std::string &name, function *f)
    : m_ctx(ctx),
      m_name(name),
      m_f(f) { f->m_blocks.push_back(this); }

std::string km2::interpreter_backend::block::name() const {
    return m_name;
}

std::ostream &km2::interpreter_backend::block::print(std::ostream &stream) const {
    return stream << "{ name: " << m_name
                  << ", instructions: " << m_instructions
                  << " }";
}
