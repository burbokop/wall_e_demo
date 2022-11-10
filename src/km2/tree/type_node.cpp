#include "type_node.h"

#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/constants_capability.h>
#include <src/km2/backend/unit/capabilities/type_capability.h>

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

wall_e::opt<uint16_t> km2::type_node::parse_integer_type(const std::string &str) {
    if(str.size() > 1) {
        try {
            return std::stoi(str.substr(1, str.size() - 1));
        }  catch (std::exception e) {
            return std::nullopt;
        }
    }
    return std::nullopt;
}

km2::markup_string km2::type_node::hover() const {
    using namespace km2::literals;
    switch (m_type) {
    case Unsigned: return "**primitive integer type** "_md + m_original_text;
    case Signed: return "**primitive integer type** "_md + m_original_text;
    case Float: return "**primitive float type** "_md + m_original_text;
    case Double: return "**primitive float type** "_md + m_original_text;
    case String: return "**primitive type** "_md + m_original_text;
    case Undefined: return "**undefined type** "_md + m_original_text;
    }
    return ""_md;
}

km2::type_node::type_node(
        const wall_e::index& index,
        type t,
        const wall_e::opt<uint16_t> &bits,
        const std::string &original_text,
        const wall_e::text_segment &segment
        )
    : km2::abstract_type_node(index, {}, segment),
      m_type(t),
      m_bits(bits),
      m_original_text(original_text) {}

wall_e::gram::argument km2::type_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(args.size() > 0) {
        const auto token = args[0].option<wall_e::lex::token>();
        if(token.has_value()) {
            if(token.value().name == "TOK_UNSIGNED") {
                return std::make_shared<type_node>(index, Unsigned, parse_integer_type(token.value().text), token.value().text, token.value().segment());
            } else if(token.value().name == "TOK_SIGNED") {
                return std::make_shared<type_node>(index, Signed, parse_integer_type(token.value().text), token.value().text, token.value().segment());
            } else if(token.value().name == "TOK_FLOAT") {
                return std::make_shared<type_node>(index, Float, std::nullopt, token.value().text, token.value().segment());
            } else if(token.value().name == "TOK_DOUBLE") {
                return std::make_shared<type_node>(index, Double, std::nullopt, token.value().text, token.value().segment());
            } else if(token.value().name == "TOK_STRING") {
                return std::make_shared<type_node>(index, String, std::nullopt, token.value().text, token.value().segment());
            }
        }
    }
    return std::make_shared<type_node>(index, Undefined, std::nullopt, std::string());
}

wall_e::either<wall_e::error, km2::backend::type*> km2::type_node::generate_backend_type(const std::shared_ptr<backend::unit> &unit) {
    if(m_type == Unsigned || m_type == Signed) {
        if(m_bits) {
            return wall_e::right<backend::type*>(unit->cap<backend::type_capability>()->custom_int(*m_bits));
        } else {
            return wall_e::left(wall_e::error("bits not set for integer type"));
        }
    } else if(m_type == Float) {
        return wall_e::right<backend::type*>(unit->cap<backend::type_capability>()->float32());
    } else if(m_type == Double) {
        return wall_e::right<backend::type*>(unit->cap<backend::type_capability>()->float64());
    } else if(m_type == String) {
        return wall_e::right<backend::type*>(unit->cap<backend::type_capability>()->cstr());
    }
    return wall_e::left(wall_e::error("unknown type_node type"));
}

void km2::type_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{type_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "type: " << type_string(m_type) << std::endl;
    stream << std::string(level + 1, ' ') << "bits: " << m_bits << std::endl;
}

wall_e::list<wall_e::error> km2::type_node::errors() const {
    return { wall_e::error("err not implemented") };
}

void km2::type_node::short_print(std::ostream &stream) const {
    stream << "type_node { type: " << m_type << ", bits: " << m_bits << " }";
}


km2::ast_token_list km2::type_node::tokens() const {
    return {
        ast_token {
            .type = AstType,
            .node_type = wall_e::type_name<type_node>(),
            .hover = hover(),
            .text = m_original_text,
            .segment = this->segment()
        }
    };
}
