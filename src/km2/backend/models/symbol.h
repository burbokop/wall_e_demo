#ifndef SYMBOL_H
#define SYMBOL_H

#include <wall_e/src/utility/collections.h>
#include <functional>

namespace km2 {
namespace backend {

struct symbol {
    struct proto {
        wall_e::str_list namespace_name;
        std::string name;
        wall_e::str_list arg_types;
        std::string ret_type;
    };
public:

    const wall_e::str_list m_namespace_name;
    const std::string m_name;
    const wall_e::str_list m_arg_types;
    const std::string m_ret_type;
    const std::string m_mangled_name;
    const std::function<std::string(const proto&)> m_mangle_func;
public:
    static std::string cppmangle(const proto& p);

    symbol(
            const wall_e::str_list& namespace_name,
            const std::string &name,
            const wall_e::str_list &arg_types,
            const std::string &ret_type,
            const std::function<std::string(const proto&)>& mangle_func = cppmangle
            );
    const wall_e::str_list& namespace_name() const;
    std::string name() const;
    const wall_e::str_list& arg_types() const;
    std::string ret_type() const;
    std::string mangled_name() const;
    std::function<std::string(const proto&)> mangle_func() const;
};

}
}
#endif // SYMBOL_H
