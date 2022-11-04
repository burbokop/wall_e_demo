#include "execution_ctx.h"

#include <iostream>
#include <sstream>

km2::interpreter_backend::execution_ctx::execution_ctx() {}

void km2::interpreter_backend::execution_ctx::fault() {
    std::cerr << "fault" << std::endl;
}

std::ostream &km2::interpreter_backend::execution_ctx::print(std::ostream & stream) const {
    stream << "instructions:" << std::endl;
    for(std::size_t i = 0; i < m_instructions.size(); ++i) {
        m_instructions[i]->print(stream << "\t#" << i << ": ") << std::endl;
    }
    return stream;
}

bool km2::interpreter_backend::execution_ctx::proceed_next() {
    if(m_verbose) std::cout << "call stack: " << m_call_stack << std::endl;

    if(m_current_index.offset < m_instructions.size()) {
        if(m_verbose) m_instructions[m_current_index.offset]->print(std::cout << "proceed instruction: " << m_current_index.offset << " -> ") << std::endl;
        const auto& res = m_instructions[m_current_index.offset]->exec({});
        if(res.value) {
            if(!m_call_stack.empty()) {
                m_call_stack.top().values.push_back(res.value);
            }
        }
        m_current_index = res.next_inst;
        return true;
    } else if(m_current_index.offset != m_current_index.null_offset) {
        std::stringstream ss;
        ss << "can not proceed instruction: " << m_current_index.offset << ". out of range" << std::endl;
        add_err(ss.str());
        fault();
    }
    return false;
}

void km2::interpreter_backend::execution_ctx::exec(const dyn_function_ptr &entry) {
    exec(entry->entry(this));
}

void km2::interpreter_backend::execution_ctx::exec(const instruction::index& entry) {
    m_current_index = entry;
    if(m_verbose) std::cout << "exec on entry: " << entry << std::endl;
    while(proceed_next()) {}
}

km2::interpreter_backend::dyn_function::native_func km2::interpreter_backend::execution_ctx::native_func(const std::string &name) const {
    return nullptr;
}
