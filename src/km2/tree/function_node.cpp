#include "decl_arg_node.h"
#include "function_node.h"
#include "namespace_node.h"


#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/translation_unit/translation_unit.h>
#include <src/km2/translation_unit/capabilities/type_capability.h>

wall_e::gram::argument km2::function_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    std::cout << "km2::function_node::create: " << args << std::endl;
    if(args.size() > 5 && args[0].contains_type<wall_e::lex::token>()) {
        std::vector<std::shared_ptr<decl_arg_node>> da_nodes;
        const auto decl_args = args[3].constrain();
        for(const auto& decl_arg : decl_args) {
            const auto da_node = decl_arg.option_cast<std::shared_ptr<km2::decl_arg_node>>();
            if(da_node.has_value()) {
                da_nodes.push_back(da_node.value());
            }
        }

        return std::make_shared<function_node>(
                    args[0].value<wall_e::lex::token>().text,
                da_nodes,
                args[5].cast_or<std::shared_ptr<abstract_value_node>>()
                );
    }
    return nullptr;
}

km2::function_node::function_node(
        const wall_e::index &index,
        const std::string &name,
        const std::vector<std::shared_ptr<decl_arg_node> > &args,
        std::shared_ptr<abstract_value_node> body
        )
    : abstract_value_node(index, cast_to_children(args, std::vector { body })),
      m_name(name),
      m_args(args),
      m_body(body) {}


wall_e::either<
wall_e::error,
llvm::Value *
> km2::function_node::generate_llvm(const std::shared_ptr<km2::translation_unit> &unit) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::vector<llvm::Type*> argTypes;
    std::vector<std::string> argNames;
    for(const auto& arg : m_args) {
        if(const auto type = arg->type_node()->generate_llvm(unit)) {

            argTypes.push_back(type.right().value());
            argNames.push_back(arg->name());
        } else {
            return type.left();
        }
    }

    std::list<std::string> namepace_name;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        namepace_name = ns->full_name();
    }

    const auto proto = unit->proto(m_name, argTypes, unit->cap<type_capability>()->void_type());

    const auto block = unit->begin_block(m_name + "_block", proto, argNames);

    if(const auto block_node = nearest_ancestor<km2::block_node>()) {
        if(const auto& error = block_node->add_function(km2::function(namepace_name, m_name, proto))) {
            return wall_e::left(*error);
        }
    }

    if (m_body) {
        if(const auto body = m_body->generate_llvm(unit)) {

        } else  {
            return body.left();
        }
    }

    unit->llvm_builder()->CreateRetVoid();
    unit->end_block();
    return wall_e::right<llvm::Value *>(proto);
}

void km2::function_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{function_node}:" << std::endl;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        stream << std::string(level + 1, ' ') << "name: " << wall_e::lex::join(ns->full_name(), "::") << "::" << m_name << std::endl;
    } else {
        stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    }
    stream << std::string(level + 1, ' ') << "args: " << std::endl;

    for(const auto& a : m_args) {
        if(a) {
            a->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') + "null arg" << std::endl;
        }
    }
    stream << std::string(level + 1, ' ') << "body: " << std::endl;
    if(m_body) {
        m_body->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "body not exist" << std::endl;
    }
}

std::list<wall_e::error> km2::function_node::errors() const {
    return {};
}
