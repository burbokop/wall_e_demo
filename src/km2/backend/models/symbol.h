#ifndef SYMBOL_H
#define SYMBOL_H

#include <list>
#include <string>
#include <functional>

namespace km2 {
namespace backend {

struct symbol {
    struct proto {
        std::list<std::string> namespace_name;
        std::string name;
        std::list<std::string> arg_types;
        std::string ret_type;
    };
public:

    const std::list<std::string> m_namespace_name;
    const std::string m_name;
    const std::list<std::string> m_arg_types;
    const std::string m_ret_type;
    const std::string m_mangled_name;
    const std::function<std::string(const proto&)> m_mangle_func;
public:
    static std::string cppmangle(const proto& p);

    symbol(
            const std::list<std::string>& namespace_name,
            const std::string &name,
            const std::list<std::string> &arg_types,
            const std::string &ret_type,
            const std::function<std::string(const proto&)>& mangle_func = cppmangle
            );
    std::list<std::string> namespace_name() const;
    std::string name() const;
    std::list<std::string> arg_types() const;
    std::string ret_type() const;
    std::string mangled_name() const;
    std::function<std::string(const proto&)> mangle_func() const;
};

}
}
#endif // SYMBOL_H
