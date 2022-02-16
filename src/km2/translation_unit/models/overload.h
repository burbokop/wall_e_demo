#ifndef OVERLOAD_H
#define OVERLOAD_H

#include <list>
#include <string>

#include <llvm/IR/Function.h>

#include <wall_e/src/models/error.h>


namespace km2 {
class function;
class overload {
    const std::list<std::string> m_namespace_stack;
    const std::string m_name;
    std::list<llvm::Function*> m_values;
public:
    overload(
            const std::list<std::string> &namespace_stack,
            const std::string &name,
            const std::list<llvm::Function*> &values
            );
    std::list<std::string> namespace_stack() const;
    std::string name() const;
    std::list<llvm::Function*> values() const;

    static bool is_same_signature(llvm::Function* f, std::vector<llvm::Type*> arg_types, llvm::Type* return_type);
    static bool is_same_signature(llvm::Function* f0, llvm::Function* f1);

    static std::shared_ptr<overload> shared_from_function(const km2::function& func);

    llvm::Function *find(std::vector<llvm::Type*> arg_types, llvm::Type* return_type = nullptr);

    std::optional<wall_e::error> add_value(llvm::Function *value, const wall_e::text_segment& segment);
};

}
#endif // OVERLOADS_H
