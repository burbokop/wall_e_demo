#include "type_node.h"


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
                return new type_node(Unsigned, token.value().text);
            } else if(token.value().name == "TOK_SIGNED") {
                return new type_node(Signed, token.value().text);
            } else if(token.value().name == "TOK_FLOAT") {
                return new type_node(Float, token.value().text);
            } else if(token.value().name == "TOK_DOUBLE") {
                return new type_node(Double, token.value().text);
            } else if(token.value().name == "TOK_STRING") {
                return new type_node(String, token.value().text);
            }
        }
    }
    return new type_node(Undefined);
}

llvm::Type *km2::type_node::generate_llvm(module_builder *builder) {
    if(m_type == Unsigned || m_type == Signed) {
        if(m_text.size() > 1) {
            try {
                return llvm::Type::getIntNTy(*builder->context(), std::stoi(m_text.substr(1, m_text.size() - 1)));
            }  catch (std::exception e) {
                return nullptr;
            }
        }
    } else if(m_type == Float) {
        return llvm::Type::getFloatTy(*builder->context());
    } else if(m_type == Double) {
        return llvm::Type::getDoubleTy(*builder->context());
    } else if(m_type == String) {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(*builder->context()), 0);
    }
    return nullptr;
}

void km2::type_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{type_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "type: " << type_string(m_type) << std::endl;
    stream << std::string(level + 1, ' ') << "text: " << m_text << std::endl;
}
