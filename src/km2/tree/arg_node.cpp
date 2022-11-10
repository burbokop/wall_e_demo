#include "arg_node.h"

#include <iostream>

#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/constants_capability.h>

#include "../backend/entities/value.h"
#include "wall_e/src/macro.h"
#include "wall_e/src/color.h"


std::string km2::arg_node::type_string(const type &t) {
    switch (t) {
    case Id: return "Id";
    case IntLiteral: return "IntLiteral";
    case FloatLiteral: return "FloatLiteral";
    case StringLiteral: return "StringLiteral";
    case ValueNode: return "ValueNode";
    case Undefined: return "Undefined";
    }
    return "Undefined";
}

km2::arg_node::arg_node(
        const wall_e::index& index,
        const wall_e::text_segment& segment,
        type t,
        const std::string& text,
        std::shared_ptr<abstract_value_node> value_node
        )
    : km2::abstract_value_node(index, { value_node }, segment),
      m_type(t),
      m_text(text),
      m_value_node(value_node) {}

wall_e::gram::argument km2::arg_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::arg_node::create: " << args << std::endl;
    if(args.size() > 0) {
        if(args[0].contains_type<wall_e::lex::token>()) {
            const auto token = args[0].value<wall_e::lex::token>();
            if(token.name == "INT_LITERAL") {
                return std::make_shared<arg_node>(index, token.segment(), IntLiteral, token.text);
            } else if(token.name == "FLOAT_LITERAL") {
                return std::make_shared<arg_node>(index, token.segment(), FloatLiteral, token.text);
            } else if(token.name == "STRING_LITERAL") {
                return std::make_shared<arg_node>(index, token.segment(), StringLiteral, wall_e::lex::parse_string_literal(token.text, true));
            } else if(token.name == "TOK_ID") {
                return std::make_shared<arg_node>(index, token.segment(), Id, token.text);
            }
        } else {
            const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();
            if (node.has_value() && node.value()) {
                return std::make_shared<arg_node>(index, node.value()->segment(), ValueNode, std::string(), node.value());
            }
        }
    }
    return std::make_shared<arg_node>(index, wall_e::text_segment(), Undefined);
}

km2::ast_token_type km2::arg_node::token_type() const {
    switch (m_type) {
    case Id: return AstParameter;
    case IntLiteral: return AstNumber;
    case FloatLiteral: return AstNumber;
    case StringLiteral: return AstString;
    case ValueNode: return wall_e::enums::max_value<ast_token_type>();
    case Undefined: return wall_e::enums::max_value<ast_token_type>();
    }
    return wall_e::enums::max_value<ast_token_type>();
}

km2::markup_string km2::arg_node::hover() const {
    using namespace km2::literals;
    switch (m_type) {
    case Id: return "**veriable** "_md + m_text;
    case IntLiteral: return "**integer** "_md + m_text;
    case FloatLiteral: return "**float** "_md + m_text;
    case StringLiteral: return "**string** '"_md + wall_e::lex::encode_special_syms(m_text) + "'";
    case ValueNode: return markup_string();
    case Undefined: return markup_string();
    }
    return {};
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::arg_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_type == IntLiteral) {
        try {
            const auto& r = unit->cap<backend::constants_capability>()->uintptr(std::stoi(m_text));
            return wall_e::right<backend::value*>(r);
        }  catch (std::exception e) {
            return wall_e::left(wall_e::error(m_text + " is not a integer"));
        }
    } else if(m_type == FloatLiteral) {
        try {
            const auto& r = unit->cap<backend::constants_capability>()->float64(std::stod(m_text));
            return wall_e::right<backend::value*>(r);
        }  catch (std::exception e) {
            return wall_e::left(wall_e::error(m_text + " is not a floating point number"));
        }
    } else if(m_type == StringLiteral) {
        //unit->setup_insert_point();
        const auto& r = unit->cap<backend::constants_capability>()->string_cstr(
                                                "arg_" + std::to_string(reinterpret_cast<uintptr_t>(this)),
                                                m_text
                                                );
        return wall_e::right<backend::value*>(r);
    } else if(m_type == Id) {
        //unit->setup_insert_point();
        if(const auto a = unit->arg(m_text)) {
            return wall_e::right<backend::value*>(a);
        } else {
            return wall_e::left(wall_e::error(
                                    "variable " + m_text + " not found in current context",
                                    wall_e::error::err,
                                    wall_e::error::semantic,
                                    0,
                                    segment()
                                    ));
        }
    } else if(m_type == ValueNode) {
        if(m_value_node) {
            //unit->setup_insert_point();
            const auto& r = m_value_node->generate_backend_value(unit);
            return r;
        } else {
            return wall_e::left(wall_e::error("empty value node"));
        }
    }

    return wall_e::left(wall_e::error("unknown arg type"));
}

void km2::arg_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{arg_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "type: " << type_string(m_type) << std::endl;
    stream << std::string(level + 1, ' ') << "text: " << m_text << std::endl;
    if (m_value_node) {
        m_value_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') << "value node is empty" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::arg_node::errors() const {
    return {};
}

void km2::arg_node::short_print(std::ostream &stream) const {
    stream << "arg_node { type: " << m_type << ", text: " << m_text << " }";
}

km2::ast_token_list km2::arg_node::tokens() const {
    const auto& encoded_text = wall_e::lex::encode_special_syms(m_text);
    if(m_type == ValueNode) {
        return m_value_node ? m_value_node->tokens() : ast_token_list {};
    } else {
        return ast_token_list {
            ast_token {
                .type = token_type(),
                .node_type = wall_e::type_name<arg_node>(),
                .hover = hover(),
                .text = encoded_text,
                .segment = this->segment()
            }
        };
    }
}
