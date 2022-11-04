#ifndef KM2_LLVM_BACKEND_NAMESPACE_CAP_H
#define KM2_LLVM_BACKEND_NAMESPACE_CAP_H

#include "../../../backend/unit/capabilities/namespace_capability.h"
#include "../../../backend/models/symbol.h"

#include <list>
#include <stack>
#include <src/km2/backend/models/function_ref.h>

namespace km2 {
namespace llvm_backend {

class namespace_capability : public backend::namespace_capability {
    struct symbol {
        std::string name;
        std::string mangled_name;
    };

    struct ctx {
        std::list<std::string> name;
        backend::value *entry = nullptr;
        std::list<symbol> symbols;
        wall_e::vec<backend::function*> functions;
        std::int64_t current_function = -1;
    };
    std::stack<ctx> m_stack;
public:
    namespace_capability(backend::unit* unit);

    virtual void begin_namespace(const std::list<std::string>& name) override;
    virtual backend::value *end_namespace() override;

    virtual std::string mangle(const std::list<std::string>& namespace_name, const std::string &sym_name) override;

    virtual std::string register_symbol(std::string sym_name) override;

    virtual void begin_function(
            backend::type* result_type,
            wall_e::vec<backend::type*> arg_types,
            const wall_e::str_list &namespace_name,
            const std::string& name,
            bool is_var_arg
            ) override;

    virtual backend::function *end_function() override;

    virtual wall_e::vec<backend::function*> find_overloads(
            const wall_e::str_list &namespace_name,
            const std::string& name
            ) override;
};

}
}
#endif // NAMESPACE_CAP_H
