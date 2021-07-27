#include "decl_arg_node.h"
#include "function_node.h"


#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/builder.h>

wall_e::gram::argument km2::function_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::function_node::create: " << args << std::endl;
    if(args.size() > 5 && args[0].contains_type<wall_e::lex::token>()) {
        std::vector<decl_arg_node*> da_nodes;
        const auto decl_args = args[3].constrain();
        for(const auto& decl_arg : decl_args) {
            const auto da_node = decl_arg.option_cast<km2::decl_arg_node*>();
            if(da_node.has_value()) {
                da_nodes.push_back(da_node.value());
            }
        }

        return new function_node(
                    args[0].value<wall_e::lex::token>().text,
                da_nodes,
                args[5].default_cast<abstract_value_node*>()
                );
    }
    return nullptr;
}

km2::function_node::function_node(const std::string &name, const std::vector<decl_arg_node *> &args, abstract_value_node *body) {
    m_name = name;
    m_args = args;
    m_body = body;
}


wall_e::either<km2::error, llvm::Value *> km2::function_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::vector<llvm::Type*> argTypes;
    std::vector<std::string> argNames;
    for(const auto& arg : m_args) {
        if(const auto type = arg->type_node()->generate_llvm(builder)) {

            argTypes.push_back(type.right().value());
            argNames.push_back(arg->name());
        } else {
            return type.left();
        }
    }
    const auto proto = builder->proto(llvm::Type::getVoidTy(*builder->context()), argTypes, m_name);
    const auto block = builder->beginBlock(m_name + "_block", proto, argNames);

    if (m_body) {
        if(const auto body = m_body->generate_llvm(builder)) {

        } else  {
            return body.left();
        }
    }

    builder->builder()->CreateRetVoid();
    builder->endBlock();
    return wall_e::right<llvm::Value *>(proto);
}

void km2::function_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{function_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    stream << std::string(level + 1, ' ') << "args: " << m_args << std::endl;
    if(m_body) {
        m_body->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "body not exist" << std::endl;
    }
}

std::list<km2::error> km2::function_node::errors() {
    return {};
}