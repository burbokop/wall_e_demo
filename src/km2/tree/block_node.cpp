#include "block_node.h"
#include  <iostream>

km2::block_node::block_node(std::shared_ptr<cmd_node> cmd, std::shared_ptr<block_node> next_node) {
    m_cmd = cmd;
    m_next_node = next_node;
}

wall_e::gram::argument km2::block_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::block_node::create" << std::endl;
    if (args.size() > 0) {
        const auto cmd = args[0].value_default<std::shared_ptr<cmd_node>>(nullptr);
        if (args.size() > 2) {
            const auto next_block = args[2].value_default<std::shared_ptr<block_node>>(nullptr);
            return std::make_shared<block_node>(cmd, next_block);
        } else {
            return std::make_shared<block_node>(cmd, nullptr);
        }
    }
    return std::make_shared<block_node>(nullptr);
}


wall_e::either<km2::error, llvm::Value *> km2::block_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_cmd) {
        if(const auto cmd_result = m_cmd->generate_llvm(builder)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_llvm(builder);
    }
    return wall_e::right<llvm::Value *>(nullptr);
}


void km2::block_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{block_node}:" << std::endl;
    if(m_cmd) {
        m_cmd->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "cmd not exist" << std::endl;
    }
    if(m_next_node) {
        m_next_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "next node not exist" << std::endl;
    }
}


std::list<km2::error> km2::block_node::errors() {
    return { error("err not implemented") };
}
