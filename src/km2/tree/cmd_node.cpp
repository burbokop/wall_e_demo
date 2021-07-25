#include "cmd_node.h"

#include <iostream>

km2::cmd_node::cmd_node(node* n) {
    m_node = n;
}

wall_e::gram::argument km2::cmd_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 0) {
        std::cout << "cmd.value: " << args[0] << "\n";
        if(args[0].contains_type<km2::node*>()) {
            const auto n = args[0].value<km2::node*>();
            std::cout << n << "\n";
            return new cmd_node(n);
        }
    }
    return new cmd_node();
}

llvm::Value *km2::cmd_node::generate_llvm(module_builder *builder) {
    return nullptr;
}


void km2::cmd_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{cmd_node}:" << std::endl;
    if(m_node) {
        m_node->print(level + 1, stream);
    } else {
        stream << std::string(level, ' ') + "node not exist" << std::endl;
    }
}
