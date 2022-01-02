#ifndef NAMESPACE_CAP_H
#define NAMESPACE_CAP_H

#include <llvm/IR/Value.h>
#include <list>
#include <stack>

namespace km2 {
class module;
class namespace_cap {
    struct symbol {
        std::string name;
        std::string mangled_name;
    };

    struct ctx {
        std::list<std::string> name;
        llvm::Value *entry = nullptr;
        std::list<symbol> m_symbols;
    };
    std::stack<ctx> m_stack;
    km2::module* m_module;
public:
    namespace_cap(km2::module* module);

    void begin_namespace(const std::list<std::string>& name);
    llvm::Value *end_namespace();

    std::string mangle(const std::list<std::string>& namespace_name, const std::string &sym_name);

    std::string register_symbol(std::string sym_name);
};

}
#endif // NAMESPACE_CAP_H
