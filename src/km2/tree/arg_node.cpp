#include "arg_node.h"

#include <iostream>

#include <src/km2/module.h>

std::string km2::arg_node::typeString(const type &t) {
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
        const wall_e::text_segment& segment,
        type t,
        const std::string& text,
        std::shared_ptr<abstract_value_node> value_node
        )
    : km2::abstract_value_node({ value_node }, segment),
      m_type(t),
      m_text(text),
      m_value_node(value_node) {}

wall_e::gram::argument km2::arg_node::create(const wall_e::gram::arg_vector &args) {
    std::cout << "km2::arg_node::create: " << args << std::endl;
    if(args.size() > 0) {
        if(args[0].contains_type<wall_e::lex::token>()) {
            const auto token = args[0].value<wall_e::lex::token>();
            if(token.name == "INT_LITERAL") {
                return std::make_shared<arg_node>(token.segment(), IntLiteral, token.text);
            } else if(token.name == "FLOAT_LITERAL") {
                return std::make_shared<arg_node>(token.segment(), FloatLiteral, token.text);
            } else if(token.name == "STRING_LITERAL") {
                return std::make_shared<arg_node>(token.segment(), StringLiteral, wall_e::lex::parse_string_literal(token.text, true));
            } else if(token.name == "TOK_ID") {
                return std::make_shared<arg_node>(token.segment(), Id, token.text);
            }
        } else {
            const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();
            if (node.has_value() && node.value()) {
                return std::make_shared<arg_node>(node.value()->segment(), ValueNode, std::string(), node.value());
            }
        }
    }
    return std::make_shared<arg_node>(wall_e::text_segment(), Undefined);
}

wall_e::either<wall_e::error, llvm::Value *> km2::arg_node::generate_llvm(const std::shared_ptr<km2::module> &module) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_type == IntLiteral) {
        try {
            return wall_e::right<llvm::Value *>(module->uintptr(std::stoi(m_text)));
        }  catch (std::exception e) {
            return wall_e::left(wall_e::error(m_text + " is not a integer"));
        }
    } else if(m_type == FloatLiteral) {
        try {
            return wall_e::right<llvm::Value *>(module->float64(std::stod(m_text)));
        }  catch (std::exception e) {
            return wall_e::left(wall_e::error(m_text + " is not a floating point number"));
        }
    } else if(m_type == StringLiteral) {
        module->setupInsertPoint();
        return wall_e::right<llvm::Value *>(module->string_const_ptr(
                                                "arg_" + std::to_string(reinterpret_cast<uintptr_t>(this)),
                                                m_text
                                                ));
    } else if(m_type == Id) {
        module->setupInsertPoint();
        if(const auto a = module->arg(m_text)) {
            return wall_e::right<llvm::Value *>(a);
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
            module->setupInsertPoint();
            return m_value_node->generate_llvm(module);
        } else {
            return wall_e::left(wall_e::error("empty value node"));
        }
    }

    return wall_e::left(wall_e::error("unknown arg type"));
}

void km2::arg_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{arg_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "type: " << typeString(m_type) << std::endl;
    stream << std::string(level + 1, ' ') << "text: " << m_text << std::endl;
    if (m_value_node) {
        m_value_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') << "value node is empty" << std::endl;
    }
}


std::list<wall_e::error> km2::arg_node::errors() {
    return {};
}
