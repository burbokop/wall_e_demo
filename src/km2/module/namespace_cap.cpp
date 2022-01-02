#include "module.h"
#include "namespace_cap.h"

km2::namespace_cap::namespace_cap(km2::module* module) : m_module(module) {}

void km2::namespace_cap::begin_namespace(const std::list<std::string> &name) {
    if(m_stack.size() == 0) {
        m_stack.push({ .name = name, .entry = m_module->beginEntry("main") });
    } else {
        m_stack.push({ .name = name });
    }
}

llvm::Value * km2::namespace_cap::end_namespace() {
    const auto entry = m_stack.top().entry;
    if(entry) {
        m_module->builder()->CreateRet(m_module->uint32(0));
        m_module->endBlock();
    }
    m_stack.pop();
    return entry;
}

std::string km2::namespace_cap::mangle(const std::list<std::string> &namespace_name, const std::string &sym_name) {


     std::to_string(namespace_name.size())

    return wall_e::lex::join(namespace_name, "_") + "_" + sym_name;
}

std::string km2::namespace_cap::register_symbol(std::string sym_name) {
    if(m_stack.size() > 0) {
        m_stack.top().m_symbols.push_back({ .name = sym_name });
        return ;
    } else {
        return std::string();
    }
}
