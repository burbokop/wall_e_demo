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

std::optional<uint16_t> km2::type_node::parse_integer_type(const std::string &str) {
    if(str.size() > 1) {
        try {
            return std::stoi(str.substr(1, str.size() - 1));
        }  catch (std::exception e) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

km2::type_node::type_node(type t, const std::optional<uint16_t> &bits)
    : m_type(t),
      m_bits(bits) {}

wall_e::gram::argument km2::type_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 0) {
        const auto token = args[0].option<wall_e::lex::token>();
        if(token.has_value()) {
            if(token.value().name == "TOK_UNSIGNED") {
                return std::make_shared<type_node>(Unsigned, parse_integer_type(token.value().text));
            } else if(token.value().name == "TOK_SIGNED") {
                return std::make_shared<type_node>(Signed, parse_integer_type(token.value().text));
            } else if(token.value().name == "TOK_FLOAT") {
                return std::make_shared<type_node>(Float, std::nullopt);
            } else if(token.value().name == "TOK_DOUBLE") {
                return std::make_shared<type_node>(Double, std::nullopt);
            } else if(token.value().name == "TOK_STRING") {
                return std::make_shared<type_node>(String, std::nullopt);
            }
        }
    }
    return std::make_shared<type_node>(Undefined, std::nullopt);
}

wall_e::either<km2::error, llvm::Type *> km2::type_node::generate_llvm(const std::shared_ptr<km2::module> &module) {
    if(m_type == Unsigned || m_type == Signed) {
        if(m_bits) {
            return wall_e::right<llvm::Type*>(llvm::Type::getIntNTy(*module->context(), *m_bits));
        } else {
            return wall_e::left(km2::error("bits not set for integer type"));
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
    stream << std::string(level + 1, ' ') << "bits: " << m_bits << std::endl;
}


std::list<km2::error> km2::type_node::errors() {
    return { error("err not implemented") };
}
