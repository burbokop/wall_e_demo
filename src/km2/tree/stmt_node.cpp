#include "call_node.h"
#include "stmt_node.h"
#include "proto_node.h"

#include <iostream>

km2::stmt_node::stmt_node(std::shared_ptr<abstract_value_node> node)
    : km2::abstract_value_node({ node }),
    m_node(node) {}

wall_e::gram::argument km2::stmt_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << wall_e::type_name<stmt_node>() << "::create: " << args << std::endl;
    if(args.size() > 0) {
        std::cout << "km2::cmd_node::create 1: " << args[0] << std::endl;
        const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();
        if(node.has_value()) {
            std::cout << "km2::cmd_node::create 2: " << node.value() << std::endl;
            return std::make_shared<stmt_node>(node.value());
        }
    }
    return std::make_shared<stmt_node>();
}

wall_e::either<wall_e::error, llvm::Value *> km2::stmt_node::generate_llvm(const std::shared_ptr<module> &module) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_node) {
        return m_node->generate_llvm(module);
    }
    return wall_e::left(wall_e::error("cmd node not exist"));
}


void km2::stmt_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{stmt_node}:" << std::endl;
    if(m_node) {
        m_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "node not exist" << std::endl;
    }
}


std::list<wall_e::error> km2::stmt_node::errors() {
    return { wall_e::error("err not implemented") };
}
