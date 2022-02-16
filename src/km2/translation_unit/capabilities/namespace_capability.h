#ifndef NAMESPACE_CAP_H
#define NAMESPACE_CAP_H

#include "abstract_capability.h"
#include "../models/symbol.h"

#include <llvm/IR/Value.h>
#include <list>
#include <stack>
#include <src/km2/translation_unit/models/function.h>

namespace km2 {

class namespace_capability : public abstract_capability {
    struct symbol {
        std::string name;
        std::string mangled_name;
    };

    struct ctx {
        std::list<std::string> name;
        llvm::Value *entry = nullptr;
        std::list<symbol> symbols;
        std::vector<llvm::Function*> functions;
        std::int64_t current_function = -1;
    };
    std::stack<ctx> m_stack;
public:
    namespace_capability(km2::translation_unit* unit);

    void begin_namespace(const std::list<std::string>& name);
    llvm::Value *end_namespace();

    std::string mangle(const std::list<std::string>& namespace_name, const std::string &sym_name);

    std::string register_symbol(std::string sym_name);

    void begin_function(
            llvm::Type* result_type,
            std::vector<llvm::Type*> arg_types,
            const std::list<std::string> &namespace_name,
            const std::string& name,
            bool is_var_arg
            );

    llvm::Function *end_function();

    std::vector<llvm::Function*> find_overloads(
            const std::list<std::string> &namespace_name,
            const std::string& name
            );
};

}
#endif // NAMESPACE_CAP_H
