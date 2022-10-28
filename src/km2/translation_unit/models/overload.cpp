#include "function.h"
#include "overload.h"
#include <wall_e/src/private/gram_private.h>

#include <iostream>

km2::overload::overload(const std::list<std::string> &namespace_stack, const std::string &name, const std::list<llvm::Function *> &values)
    : m_namespace_stack(namespace_stack),
      m_name(name),
      m_values(values) {}

std::list<std::string> km2::overload::namespace_stack() const { return m_namespace_stack; }
std::string km2::overload::name() const { return m_name; }
std::list<llvm::Function *> km2::overload::values() const { return m_values; }

bool km2::overload::is_same_signature(llvm::Function *f, std::vector<llvm::Type *> arg_types, llvm::Type *return_type) {
    const bool arg_count_match = f->isVarArg() ? f->arg_size() <= arg_types.size() : f->arg_size() == arg_types.size();
    if((!return_type || f->getReturnType()->getTypeID() == return_type->getTypeID()) && arg_count_match) {
        for(std::size_t i = 0; i < f->arg_size(); ++i) {
            if(f->getArg(i)->getType()->getTypeID() != arg_types[i]->getTypeID()) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool km2::overload::is_same_signature(llvm::Function *f0, llvm::Function *f1) {
    if(f0->getReturnType()->getTypeID() == f1->getReturnType()->getTypeID() && f0->arg_size() == f1->arg_size()) {
        for(std::size_t i = 0; i < f0->arg_size(); ++i) {
            if(f0->getArg(i)->getType() != f1->getArg(i)->getType()) {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::shared_ptr<km2::overload> km2::overload::shared_from_function(const function &func) {
    return std::make_shared<km2::overload>(
                func.namespace_stack(),
                func.name(),
                std::list { func.value() }
                );
}

llvm::Function *km2::overload::find(std::vector<llvm::Type *> arg_types, llvm::Type *return_type) {
    for(const auto& v : m_values) {
        if(is_same_signature(v, arg_types, return_type)) {
            return v;
        }
    }
    return nullptr;
}

std::optional<wall_e::error> km2::overload::add_value(llvm::Function *value, const wall_e::text_segment &segment) {
    for(const auto& v : m_values) {
        if(is_same_signature(v, value)) {
            return wall_e::error("same function already exists", wall_e::error::err, wall_e::error::semantic, 0, segment);
        }
    }
    m_values.push_back(value);
    return std::nullopt;
}

std::ostream &km2::overload::print(std::ostream &stream) const {
    return stream << m_namespace_stack << "::" << m_name;
}

