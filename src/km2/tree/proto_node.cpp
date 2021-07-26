#include "decl_arg_node.h"
#include "proto_node.h"
#include <wall_e/src/utility/token_tools.h>

#include <iostream>


km2::proto_node::proto_node(const std::string &name, const std::vector<decl_arg_node *> &args, abstract_type_node *result_type_node) {
    m_name = name;
    m_args = args;
    m_result_type_node = result_type_node;
}

wall_e::gram::argument km2::proto_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::proto_node::create: " << args << std::endl;

    if(args.size() > 4 && args[0].contains_type<wall_e::lex::token>()) {
        std::vector<decl_arg_node*> da_nodes;
        const auto decl_args = args[3].constrain();
        for(const auto& decl_arg : decl_args) {
            const auto da_node = decl_arg.option_cast<km2::decl_arg_node*>();
            if(da_node.has_value()) {
                da_nodes.push_back(da_node.value());
            }
        }

        return new proto_node(
                args[0].value<wall_e::lex::token>().text,
                da_nodes,
                args[4].default_cast<abstract_type_node*>()
                );
    }
    return nullptr;
}

llvm::Value *km2::proto_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::vector<llvm::Type*> argTypes;
    bool isVarArg = false;
    for(const auto& arg : m_args) {
        if(arg->is_variadic()) {
            if(isVarArg) {
                std::cerr << "... repeaded while function declaration\n";
            }
            isVarArg = true;
        } else {
            argTypes.push_back(arg->type_node()->generate_llvm(builder));
        }
    }

    if(m_result_type_node) {
        std::cout << "GENERATED 0 " << __PRETTY_FUNCTION__ << std::endl;
        if (const auto rt = m_result_type_node->generate_llvm(builder)) {
            std::cout << "GENERATED 1 " << __PRETTY_FUNCTION__ << std::endl;
            return builder->proto(rt, argTypes, m_name, isVarArg);
        }
    }
    return nullptr;
}

void km2::proto_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{proto_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    for(const auto& arg : m_args) {
        if(arg) {
            arg->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') << "arg node missing" << std::endl;
        }
    }
    if(m_result_type_node) {
        m_result_type_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') << "result type node missing" << std::endl;
    }
}
