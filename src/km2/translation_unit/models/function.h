#ifndef FUNCTION_H
#define FUNCTION_H

#include <llvm/IR/Function.h>

#include <list>

namespace km2 {

class function {
    const std::list<std::string> m_namespace_stack;
    const std::string m_name;
    llvm::Function* const m_value = nullptr;
public:
    function(const std::list<std::string> &namespace_stack, const std::string &name, llvm::Function* value);
    llvm::Function *value() const;
    std::string name() const;
    std::list<std::string> namespace_stack() const;
};

}
#endif // FUNCTION_H
