#include "call_node.h"
#include "cmd_node.h"
#include "proto_node.h"

#include <iostream>

km2::cmd_node::cmd_node(std::shared_ptr<abstract_value_node> node) {
    m_node = node;
}

wall_e::gram::argument km2::cmd_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::cmd_node::create: " << args << std::endl;
    if(args.size() > 0) {
        const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();


        std::cout << "node: " << node << "\n";
        std::cout << "opt: " << args[0].option<std::shared_ptr<proto_node>>() << "\n";
        std::cout << "lineage: " << args[0].lineage() << "\n";
        if(node.has_value()) {
            return std::make_shared<cmd_node>(node.value());
        }
    }
    return std::make_shared<cmd_node>();
}

wall_e::either<km2::error, llvm::Value *> km2::cmd_node::generate_llvm(const std::shared_ptr<module> &module) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_node) {
        return m_node->generate_llvm(module);
    }
    return wall_e::left(km2::error("cmd node not exist"));
}


void km2::cmd_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{cmd_node}:" << std::endl;
    if(m_node) {
        m_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "node not exist" << std::endl;
    }
}


std::list<km2::error> km2::cmd_node::errors() {
    return { error("err not implemented") };
}
