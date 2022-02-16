#include "constants_capability.h"
#include "namespace_capability.h"
#include "type_capability.h"
#include "../translation_unit.h"

#include "llvm/IR/Mangler.h"


km2::namespace_capability::namespace_capability(km2::translation_unit* unit)
    : abstract_capability(unit) {}

void km2::namespace_capability::begin_namespace(const std::list<std::string> &name) {
    if(m_stack.size() == 0) {
        m_stack.push({ .name = name, .entry = unit()->begin_entry("main") });
    } else {
        m_stack.push({ .name = name });
    }
}

llvm::Value *km2::namespace_capability::end_namespace() {
    const auto entry = m_stack.top().entry;
    if(entry) {
        unit()->llvm_builder()->CreateRet(unit()->cap<constants_capability>()->uint32(0));
        unit()->end_block();
    }
    m_stack.pop();
    return entry;
}

std::string km2::namespace_capability::mangle(const std::list<std::string> &namespace_name, const std::string &sym_name) {


    const auto f = llvm::Function::Create(
                llvm::FunctionType::get(
                    unit()->cap<type_capability>()->int32(),
                    { unit()->cap<type_capability>()->int32() },
                    false //is_var_arg
                    ),
                llvm::Function::ExternalLinkage,
                sym_name,
                unit()->llvm_module()
                );

    llvm::Mangler mangler;
    std::string out;
    llvm::raw_string_ostream ss(out);
    mangler.getNameWithPrefix(ss, f, false);
    return out;
}

std::string km2::namespace_capability::register_symbol(std::string sym_name) {
    if(m_stack.size() > 0) {
        m_stack.top().symbols.push_back({ .name = sym_name });
        return sym_name;
    } else {
        return std::string();
    }
}

void km2::namespace_capability::begin_function(llvm::Type *result_type, std::vector<llvm::Type *> arg_types, const std::list<std::string> &namespace_name, const std::string &name, bool is_var_arg) {
    if(m_stack.size() > 0) {
        const auto& function = unit()->proto(name, arg_types, result_type, is_var_arg);
        unit()->begin_block(name, function);
        m_stack.top().functions.push_back(function);
        m_stack.top().current_function++;
    }
}

llvm::Function* km2::namespace_capability::end_function() {
    if(m_stack.size() > 0) {
        auto& top = m_stack.top();
        if(top.current_function >= 0 && top.current_function < top.functions.size()) {
            return top.functions.at(top.current_function--);
        }
    }
    return nullptr;
}

std::vector<llvm::Function*> km2::namespace_capability::find_overloads(const std::list<std::string> &namespace_name, const std::string &name) {
    std::vector<llvm::Function*> result;
    if(m_stack.size() > 0) {
        auto& top = m_stack.top();
        for(const auto& func : top.functions) {
            if(func /* TODO */) {
                result.push_back(func);
            }
        }
    }
    return result;
}
