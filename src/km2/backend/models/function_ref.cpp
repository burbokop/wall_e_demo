#include "function_ref.h"


km2::backend::function_ref::function_ref(const wall_e::str_list &namespace_stack,
        const std::string &name,
        function* const value
        )
    : m_namespace_stack(namespace_stack),
      m_name(name),
      m_value(value) {}


