#include "base_node.h"


#include <iostream>

#include <src/km2/builder.h>


km2::base_node::base_node(block_node *block_node) {
    m_block_node = block_node;
}

wall_e::gram::argument km2::base_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::base_node::create: " << args << std::endl;
    if (args.size() > 0) {
        if(args[0].contains_type<block_node*>()) {
            return new base_node(args[0].value<block_node*>());
        }
    }
    return new base_node(nullptr);
}

wall_e::either<km2::error, llvm::Value *> km2::base_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_block_node) {
        const auto entry = builder->beginEntry("main");
        const auto result = m_block_node->generate_llvm(builder);
        builder->builder()->CreateRet(builder->uint32(0));
        builder->endBlock();
        return result;
    }
    return wall_e::right<llvm::Value *>(nullptr);
}

void km2::base_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{base_node}:" << std::endl;
    if(m_block_node) {
        m_block_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "block node not exist" << std::endl;
    }
}


std::list<km2::error> km2::base_node::errors() {
    return {};
}
