#ifndef FUNCTION_REF_H
#define FUNCTION_REF_H

#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class function;

class function_ref {
    const wall_e::str_list m_namespace_stack;
    const std::string m_name;
    function* const m_value = nullptr;
public:
    function_ref(
            const wall_e::str_list &namespace_stack,
            const std::string &name,
            function* const value
            );

    const std::string& name() const { return m_name; }
    const wall_e::str_list& namespace_stack() const { return m_namespace_stack; }
    function *const value() const { return m_value; }
};

}
}

#endif // FUNCTION_REF_H
