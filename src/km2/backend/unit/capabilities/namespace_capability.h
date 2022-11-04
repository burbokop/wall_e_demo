#ifndef KM2_BACKEND_NAMESPACE_CAP_H
#define KM2_BACKEND_NAMESPACE_CAP_H

#include "abstract_capability.h"


#include <list>
#include <stack>
#include <memory>
#include "../../entities/function.h"
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class value;
class type;

class namespace_capability : public abstract_capability {
public:
    namespace_capability(class unit* unit);

    virtual void begin_namespace(const std::list<std::string>& name) = 0;
    virtual km2::backend::value* end_namespace() = 0;

    virtual std::string mangle(const std::list<std::string>& namespace_name, const std::string &sym_name) = 0;

    virtual std::string register_symbol(std::string sym_name) = 0;

    virtual void begin_function(
            type* result_type,
            wall_e::vec<type*> arg_types,
            const wall_e::str_list &namespace_name,
            const std::string& name,
            bool is_var_arg
            ) = 0;

    virtual function* end_function() = 0;

    virtual wall_e::vec<function*> find_overloads(
            const wall_e::str_list &namespace_name,
            const std::string& name
            ) = 0;
};

}
}
#endif // NAMESPACE_CAP_H
