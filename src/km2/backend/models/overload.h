#ifndef OVERLOAD_H
#define OVERLOAD_H

#include <list>
#include <string>

#include <memory>
#include <wall_e/src/models/error.h>
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class function;
class function_ref;
class type;

class overload {
    const wall_e::str_list m_namespace_stack;
    const std::string m_name;
    wall_e::list<function*> m_values;
public:
    overload(
            const wall_e::str_list &namespace_stack,
            const std::string &name,
            const wall_e::list<function*> &values
            );
    wall_e::str_list namespace_stack() const { return m_namespace_stack; }
    std::string name() const { return m_name; }
    const wall_e::list<function*> &values() const { return m_values; }

    static bool is_same_signature(function* f, wall_e::vec<type*> arg_types, type* return_type);
    static bool is_same_signature(function* f0, function* f1);

    static std::shared_ptr<overload> shared_from_function(const function_ref& func);

    function* find(wall_e::vec<type*> arg_types, type* return_type = nullptr);

    wall_e::opt<wall_e::error> add_value(function* value, const wall_e::text_segment& segment);

    std::ostream& print(std::ostream& stream) const;
    inline friend std::ostream& operator<<(std::ostream& stream, const overload& o) { return o.print(stream); }
    inline friend std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<overload>& o) { return o->print(stream); }
};

}
}
#endif // OVERLOADS_H
