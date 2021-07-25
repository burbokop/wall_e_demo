#include "call_node.h"

#include <wall_e/src/utility/token_tools.h>
#include <iostream>

km2::call_node::call_node(const std::string &name, const std::vector<wall_e::lex::token>& args) {
    m_name = name;
    m_args = args;
}

wall_e::gram::argument km2::call_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::call_node::create" << std::endl;
    const auto function_name_token = args[0].value<wall_e::lex::token>();
    const auto constrained_args = args[2].constrain();
    const auto function_args = wall_e::remove_tokens(constrained_args, { "EP" });

    std::vector<wall_e::lex::token> argTokens;
    argTokens.reserve(function_args.size());
    for(const auto& arg : function_args) {
        if(arg.contains_type<wall_e::lex::token>()) {
            argTokens.push_back(arg.value<wall_e::lex::token>());
        }
    }

    return new call_node(function_name_token.text, argTokens);
}


llvm::Value *km2::call_node::generate_llvm(module_builder *builder) {
    const auto proto = builder->module()->getFunction(m_name);
    if (!proto) {
        std::cerr << "function: " << m_name << " not declared" << std::endl;
        return nullptr;
    }

    std::vector<llvm::Value*> args;
    for(const auto& arg : m_args) {
        args.push_back(builder->value(arg));
    }

    return builder->builder()->CreateCall(proto, args);
}



void km2::call_node::print(size_t level, std::ostream &stream)
{
}
