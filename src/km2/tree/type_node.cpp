#include "type_node.h"

#include <llvm/IR/Type.h>

#include <src/km2/module.h>

std::string km2::type_node::type_string(type t) {
    switch (t) {
    case Unsigned: return "Unsigned";
    case Signed: return "Signed";
    case Float: return "Float";
    case Double: return "Double";
    case String: return "String";
    case Undefined: return "Undefined";
    }
    return "Undefined";
}

km2::type_node::type_node(type t, const std::string &text) {
    m_type = t;
    m_text = text;
}

wall_e::gram::argument km2::type_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 0) {
        const auto token = args[0].option<wall_e::lex::token>();
        if(token.has_value()) {
            if(token.value().name == "TOK_UNSIGNED") {
                return std::make_shared<type_node>(Unsigned, token.value().text);
            } else if(token.value().name == "TOK_SIGNED") {
                return std::make_shared<type_node>(Signed, token.value().text);
            } else if(token.value().name == "TOK_FLOAT") {
                return std::make_shared<type_node>(Float, token.value().text);
            } else if(token.value().name == "TOK_DOUBLE") {
                return std::make_shared<type_node>(Double, token.value().text);
            } else if(token.value().name == "TOK_STRING") {
                return std::make_shared<type_node>(String, token.value().text);
            }
        }
    }
    return std::make_shared<type_node>(Undefined);
}

wall_e::either<km2::error, llvm::Type *> km2::type_node::generate_llvm(const std::shared_ptr<km2::module> &module) {
    if(m_type == Unsigned || m_type == Signed) {
        if(m_text.size() > 1) {
            try {
                return wall_e::right<llvm::Type*>(llvm::Type::getIntNTy(*module->context(), std::stoi(m_text.substr(1, m_text.size() - 1))));
            }  catch (std::exception e) {
                return wall_e::left(km2::error("can not parse integer type"));
            }
        }
    } else if(m_type == Float) {
        return wall_e::right<llvm::Type*>(llvm::Type::getFloatTy(*module->context()));
    } else if(m_type == Double) {
        return wall_e::right<llvm::Type*>(llvm::Type::getDoubleTy(*module->context()));
    } else if(m_type == String) {
        return wall_e::right<llvm::Type*>(llvm::PointerType::get(llvm::Type::getInt8Ty(*module->context()), 0));
    }
    return wall_e::left(km2::error("unknown type_node type"));
}

void km2::type_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{type_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "type: " << type_string(m_type) << std::endl;
    stream << std::string(level + 1, ' ') << "text: " << m_text << std::endl;
}


std::list<km2::error> km2::type_node::errors() {
    return { error("err not implemented") };
}
