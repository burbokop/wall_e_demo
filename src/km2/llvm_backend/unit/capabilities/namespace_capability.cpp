#include "constants_capability.h"
#include "namespace_capability.h"
#include "type_capability.h"

#include "../unit.h"
#include "../private.unit.h"
#include "../../entities/type.h"
#include "../../entities/value.h"

#include "llvm/IR/Mangler.h"

km2::llvm_backend::namespace_capability::namespace_capability(backend::unit *unit)
    : km2::backend::namespace_capability(unit) {}

void km2::llvm_backend::namespace_capability::begin_namespace(const std::list<std::string> &name) {
    if(m_stack.size() == 0) {
        m_stack.push(ctx { .name = name, .entry = unit()->begin_entry("km2_main") });
    } else {
        m_stack.push({ .name = name });
    }
}

km2::backend::value *km2::llvm_backend::namespace_capability::end_namespace() {
    const auto entry = m_stack.top().entry;
    if(entry) {
        unit()->as<class unit>()->p()->builder->CreateRet(
                    unit()->cap<constants_capability>()->uint32(0)->as<value>()->data()
                    );
        unit()->end_block();
    }
    m_stack.pop();
    return entry;
}

std::string km2::llvm_backend::namespace_capability::mangle(const std::list<std::string> &namespace_name, const std::string &sym_name) {


    const auto f = llvm::Function::Create(
                llvm::FunctionType::get(
                    unit()->cap<type_capability>()->int32()->as<class type>()->data(),
                    { unit()->cap<type_capability>()->int32()->as<class type>()->data() },
                    false //is_var_arg
                    ),
                llvm::Function::ExternalLinkage,
                sym_name,
                *unit()->as<class unit>()->p()->module
                );

    llvm::Mangler mangler;
    std::string out;
    llvm::raw_string_ostream ss(out);
    mangler.getNameWithPrefix(ss, f, false);
    return out;
}

std::string km2::llvm_backend::namespace_capability::register_symbol(std::string sym_name) {
    if(m_stack.size() > 0) {
        m_stack.top().symbols.push_back({ .name = sym_name });
        return sym_name;
    } else {
        return std::string();
    }
}

void km2::llvm_backend::namespace_capability::begin_function(km2::backend::type *result_type, wall_e::vec<backend::type*> arg_types, const wall_e::str_list &namespace_name, const std::string &name, bool is_var_arg) {
    if(m_stack.size() > 0) {
        const auto& function = unit()->proto(name, arg_types, result_type, is_var_arg);
        unit()->begin_block(name, function);
        m_stack.top().functions.push_back(function);
        m_stack.top().current_function++;
    }
}

km2::backend::function* km2::llvm_backend::namespace_capability::end_function() {
    if(m_stack.size() > 0) {
        auto& top = m_stack.top();
        if(top.current_function >= 0 && top.current_function < top.functions.size()) {
            return top.functions.at(top.current_function--);
        }
    }
    return nullptr;
}

wall_e::vec<km2::backend::function*> km2::llvm_backend::namespace_capability::find_overloads(const wall_e::str_list &namespace_name, const std::string &name) {
    wall_e::vec<backend::function*> result;
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

