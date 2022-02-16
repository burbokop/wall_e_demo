#include "function.h"


km2::function::function(const std::list<std::string> &namespace_stack, const std::string &name, llvm::Function *value)
    : m_namespace_stack(namespace_stack),
      m_name(name),
      m_value(value) {}

llvm::Function *km2::function::value() const { return m_value; }
std::list<std::string> km2::function::namespace_stack() const { return m_namespace_stack; }
std::string km2::function::name() const { return m_name; }
