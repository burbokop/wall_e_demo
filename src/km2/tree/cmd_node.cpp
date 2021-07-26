#include "call_node.h"
#include "cmd_node.h"

#include <iostream>

km2::cmd_node::cmd_node(abstract_value_node *n) {
    m_node = n;
}

wall_e::gram::argument km2::cmd_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 0) {
        if(args[0].inherited_by<km2::abstract_value_node*>()) {
            return new cmd_node(args[0].cast<km2::abstract_value_node*>());
        }
    }
    return new cmd_node();
}

llvm::Value *km2::cmd_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_node) {
        return m_node->generate_llvm(builder);
    }
    return nullptr;
}


void km2::cmd_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{cmd_node}:" << std::endl;
    if(m_node) {
        m_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "node not exist" << std::endl;
    }
}
