#include "call_node.h"

#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/builder.h>



km2::call_node::call_node(
        const std::string &name,
        const std::vector<abstract_value_node *> &args,
        std::string::size_type name_start_position,
        std::string::size_type name_end_position
        ) {
    m_name = name;
    m_args = args;
    m_name_start_position = name_start_position;
    m_name_end_position = name_end_position;
}

wall_e::gram::argument km2::call_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::call_node::create" << std::endl;
    if(args.size() > 2) {
        const auto function_name_token = args[0].value<wall_e::lex::token>();
        const auto function_name_end_token = args[1].value<wall_e::lex::token>();
        const auto constrained_args = args[2].constrain();
        const auto function_args = wall_e::remove_tokens(constrained_args, { "EP" });

        std::vector<km2::abstract_value_node*> funcArgs;
        funcArgs.reserve(function_args.size());
        for(const auto& arg : function_args) {
            if (arg.value_default<wall_e::lex::token>().name != "COMA") {
                funcArgs.push_back(arg.default_cast<km2::abstract_value_node*>());
            }
        }

        return new call_node(
                    function_name_token.text,
                    funcArgs,
                    function_name_token.position,
                    function_name_end_token.position
                    );
    }
    return nullptr;
}


wall_e::either<km2::error, llvm::Value *> km2::call_node::generate_llvm(module_builder *builder) {
    std::cout << __PRETTY_FUNCTION__ << "name: " << m_name << std::endl;
    const auto proto = builder->module()->getFunction(m_name);
    if (!proto) {
        std::cerr << "function: " << m_name << " not declared" << std::endl;
        return wall_e::left(km2::error("function '" + m_name + "' not defined", m_name_start_position, m_name_end_position));
    }

    std::vector<llvm::Value*> args;
    args.reserve(m_args.size());
    for(const auto& arg : m_args) {
        if (arg) {
            if(const auto arg_value = arg->generate_llvm(builder)) {
                args.push_back(arg_value.right_value());
            } else {
                return arg_value.left();
            }
        } else {
            args.push_back(nullptr);
        }
    }

    for(size_t i = 0, s = proto->arg_size(); i < s; ++i) {
        const auto func_type = proto->getArg(i)->getType();
        const auto arg_type = args[i]->getType();
        if(func_type->getTypeID() != arg_type->getTypeID()) {
            llvm::errs() << "arg " << i << " type not match: func: " << m_name << " expected type: " << *func_type << ", actual: " << *arg_type << "\n";

            std::string err_message;
            llvm::raw_string_ostream err_message_stream(err_message);
            err_message_stream
                    << "arg type '" << *arg_type
                    << "' does not match with '" << *func_type
                    << "' of func '" << m_name
                    << "'";

            return wall_e::left(km2::error(
                                    err_message,
                                    m_name_start_position,
                                    m_name_end_position
                                    ));
        }
    }

    builder->setupInsertPoint();

    return wall_e::right<llvm::Value *>(builder->builder()->CreateCall(proto, args));
}



void km2::call_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{call_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    for(const auto arg : m_args) {
        if(arg) {
            arg->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') << "null arg" << std::endl;
        }
    }
}


std::list<km2::error> km2::call_node::errors() {
    return {};
}
