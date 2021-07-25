#include "function_node.h"


#include <wall_e/src/utility/token_tools.h>
#include <iostream>

const std::string &km2::function_node::name() const {
    return m_name;
}

const std::vector<std::string> &km2::function_node::argTypes() const {
    return m_argTypes;
}


wall_e::gram::argument km2::function_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::function_node::create: " << args << std::endl;
    if(args.size() > 5 && args[0].contains_type<wall_e::lex::token>()) {
        const auto body = args[5].option<node*>();
        if(body.has_value()) {
            return new function_node(
                    args[0].value<wall_e::lex::token>().text,
                    wall_e::lex::texts(wall_e::left_every_second(args[3])),
                    body.value()
                    );

        }

    }
    return nullptr;
}

km2::function_node::function_node(const std::string &name, const std::vector<std::string> &typenames, node *body) {
    m_name = name;
    m_argTypes = typenames;
    m_body = body;
}


llvm::Value *km2::function_node::generate_llvm(module_builder *builder) {
    std::vector<llvm::Type*> argTypes;
    for(const auto& t : m_argTypes) {
        argTypes.push_back(builder->type(t));
    }
    const auto proto = builder->proto(llvm::Type::getVoidTy(*builder->context()), argTypes, m_name);
    const auto block = builder->beginBlock(m_name + "_block", proto);

    if (m_body) {
         const auto body = m_body->generate_llvm(builder);
    }
    return proto;
}

void km2::function_node::print(size_t level, std::ostream &stream) {

}
