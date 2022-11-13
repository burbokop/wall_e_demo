#include "km2.h"
#include "backend/unit/unit.h"

#include <regex>
#include <list>
#include <vector>
#include <cassert>
#include <thread>
#include <variant>
#include <iostream>
#include <fstream>

#include <wall_e/src/gram.h>
#include <wall_e/src/macro.h>
#include <wall_e/src/utility/tree_view_tools.h>
#include <wall_e/src/lex.h>
#include <src/km2/utility/math_patterns.h>
#include <src/km2/utility/function.h>
#include <wall_e/src/utility/token_tools.h>
#include <wall_e/src/private/gram_smp.h>
#include "tree/namespace_node.h"
#include "tree/call_node.h"
#include "tree/function_node.h"
#include "tree/arg_node.h"
#include "tree/proto_node.h"
#include "tree/type_node.h"
#include "tree/decl_arg_node.h"
#include "tree/const_node.h"
#include "tree/imp_node.h"
#include <wall_e/src/color.h>

#include "backend/backend.h"

struct __km2_flags_private {
    bool verbose = false;
    bool tree_mode = false;
};

__km2_flags_private __km2_parse_flags(const km2::flags &flags) {
    __km2_flags_private result;
    for(auto f : flags) {
        if(f == km2::flag::Verbose) {
            result.verbose = true;
        } else if(f == km2::flag::OnlyTree) {
            result.tree_mode = true;
        }
    }
    return result;
}





std::ostream& km2::operator<<(std::ostream& stream, const compilation_result& res) {
    return stream << "{ tokens: " << res.tokens()
                  << ", rules: " << res.rules()
                  << ", token_tree: " << res.token_tree()
                  << ", root_node: " << res.root_node()
                  << ", unit: " << res.unit()
                  << ", backend_value: " << res.backend_value()
                  << ", errors: " << res.errors()
                  << ", ast_tokens: " << res.ast_tokens()
                  << ", hovers: " << res.hovers()
                  << " }";
}




static const wall_e::lex::pattern_list& km2_lexlist() {
    static const wall_e::lex::pattern_list res = {
        { std::regex("\\/\\/(.*)\n"), wall_e::lex::special::ignore },
        { std::regex("\\/\\*(.*)\\*\\/"), wall_e::lex::special::ignore },
        { wall_e::lex::keyword("asm"), "TOK_ASM" },
        { wall_e::lex::keyword("number"), "TOK_NUMBER" },
        { wall_e::lex::keyword("string"), "TOK_STRING" },
        { wall_e::lex::keyword("const"), "TOK_CONST" },
        { wall_e::lex::keyword("(namespace|nspace)"), "TOK_NAMESPACE" },
        { wall_e::lex::keyword("exp"), "TOK_EXP" },
        { wall_e::lex::keyword("imp"), "TOK_IMP" },
        { std::regex("u[0-9]+"), "TOK_UNSIGNED" },
        { std::regex("i[0-9]+"), "TOK_SIGNED" },
        { std::regex("f32"), "TOK_FLOAT" },
        { std::regex("f64"), "TOK_DOUBLE" },
        { std::regex("'[^']*'"), "STRING_LITERAL" },
        { std::regex("[a-zA-Z_][a-zA-Z0-9_]*"), "TOK_ID" },
        { std::regex("[0-9]+\\.([0-9]+)?"), "FLOAT_LITERAL" },
        { std::regex("[0-9]+"), "INT_LITERAL" },
        { std::regex("[(]"), "OP" },
        { std::regex("[)]"), "EP" },
        { std::regex("[{]"), "OB" },
        { std::regex("[}]"), "EB" },
        { std::regex("=="), "DOUBLE_EQUALS" },
        { std::regex("="), "EQUALS" },
        { std::regex("[+]"), "TOK_PLUS" },
        { std::regex("[-]"), "TOK_MINUS" },
        { std::regex("[*]"), "TOK_MUL" },
        { std::regex("[/]"), "TOK_DIV" },
        { std::regex(";"), "SEMICOLON" },
        { std::regex("::"), "DOUBLE_COLON" },
        { std::regex(":"), "COLON" },
        { std::regex(","), "COMA" },
        { std::regex("\\.\\.\\."), "THREE_DOT" },
        { std::regex("[ \t\n]+"), wall_e::lex::special::ignore }
    };
    return res;
}

km2::compilation_result km2::compile(const backend::backend *b, std::istream &input, const flags &flags) {
    return compile(b,
            std::string(
                (std::istreambuf_iterator<char>(input)),
                (std::istreambuf_iterator<char>())
                ),
            flags
            );
}

km2::compilation_result km2::compile(const backend::backend* b, const std::string &input, const km2::flags &flags) {
    using namespace wall_e::gram::literals;

    const auto __flags = __km2_parse_flags(flags);
    wall_e::gram::flags_list gram_flags;
    if(__flags.tree_mode) {
        gram_flags.push_back(wall_e::gram::UseDefaultParser);
    }
    gram_flags.push_back(wall_e::gram::UnconditionalTransition);

    if(__flags.verbose)
        std::cout << ">>> INPUT TEXT <<<\n\n" << input << "\n\n>>> ----- ---- <<<\n";

    const auto tokens = wall_e::lex::v2::make_tokents(input, km2_lexlist());

    if(__flags.verbose) {
        for(const auto& t : tokens)
            std::cout << t.position << " " << t.name << " " << t.text << "\n";
    }

    const auto sorted_tokens = tokens;
    //const auto sorted_tokens = wall_e::lex::sort_tokens(tokens, input);

    if(__flags.verbose) {
        std::cout << std::endl;
        for(const auto& t : sorted_tokens)
            std::cout << "sorted: " << t.position << " " << t.name << " " << t.text << "\n";
    }

    wall_e::list<wall_e::error> lex_errors;
    for(const auto& st : sorted_tokens) {
        if(const auto& err = st.undef_error()) {
            if(__flags.verbose) {
                std::cout << "[ LEX ERROR ] " << *err << std::endl;
            }
            lex_errors.push_back(*err);
        }
    }

    if(lex_errors.size() > 0) {
        return compilation_result(sorted_tokens, {}, {}, {}, {}, {}, lex_errors, {});
    }

    wall_e::gram::pattern_list gram_list;

    //TOK_EXP
    //TOK_IMP
    gram_list.push_back("namespace << (TOK_EXP | -) & TOK_NAMESPACE & (TOK_ID & OB | OB) & (EB | block)"_pattern
        << km2::namespace_node::create("TOK_EXP", "TOK_ID"));

    gram_list.push_back("block << (namespace | stmt) & SEMICOLON & (EB | block)"_pattern
        << km2::block_node::create);

    //gram_list.push_back("internal_block << (namespace | stmt) & SEMICOLON & (EB | internal_block)"_pattern
    //    << km2::internal_block_node::create);

    gram_list.push_back("stmt << import | function_call | function_declaration | proto_declaration | const"_pattern
        << km2::stmt_node::create);

    gram_list.push_back("import << TOK_IMP & TOK_ID"_pattern
        << km2::imp_node::create);

    gram_list.push_back("const << TOK_CONST & TOK_ID & EQUALS & arg"_pattern
        << km2::const_node::create);

    gram_list.push_back("function_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & OB & (EB | block)"_pattern
        << km2::function_node::create);

    gram_list.push_back("proto_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & type"_pattern
        << km2::proto_node::create);

    gram_list.push_back("decl_arg_list << decl_arg & (EP | (COMA & decl_arg_list))"_pattern);

    gram_list.push_back("decl_arg << TOK_ID & (type | THREE_DOT)"_pattern
        << km2::decl_arg_node::create);

    gram_list.push_back("type << TOK_UNSIGNED | TOK_SIGNED | TOK_FLOAT | TOK_DOUBLE | TOK_STRING"_pattern
        << km2::type_node::create);

    gram_list.push_back("namespace_appeal << TOK_ID & DOUBLE_COLON & (namespace_appeal | -)"_pattern
                        );

    gram_list.push_back("function_call << (namespace_appeal | -) & TOK_ID & OP & (EP | arg_list)"_pattern
        << km2::call_node::create);

    gram_list.push_back("arg_list << arg & (EP | (COMA & arg_list))"_pattern);

    gram_list.push_back("arg << function_call | const | TOK_ID | STRING_LITERAL | FLOAT_LITERAL | INT_LITERAL"_pattern
        << km2::arg_node::create);


    //gram_list.push_back("expression << plus_munus"_pattern);
    //gram_list.push_back("plus_munus << arg & (TOK_PLUS | TOK_MINUS) & arg"_pattern);

    //gram_list.push_back("expression << plus_munus | term"_pattern);
    //
    //gram_list.push_back("plus_munus << term & (TOK_PLUS | TOK_MINUS) & expression"_pattern);
    //
    //gram_list.push_back("term << mul_div | arg");
    //
    //gram_list.push_back("mul_div << arg & (TOK_DIV | TOK_MUL) & term");

    if(__flags.verbose) {
        std::cout << "\n -------------- GRAMATIC --------------\n\n";
        std::cout << "flags: " << gram_flags << std::endl;
    }

    gram_list = wall_e::gram::pattern::simplified(gram_list);

    std::shared_ptr<wall_e::gram::log> log;
    if(__flags.verbose) {
        log = std::make_shared<wall_e::gram::log>();
    }

    auto result = wall_e::gram::exec(
                gram_list,
                sorted_tokens,
                gram_flags, wall_e::smp::simplify,
                log.get()
                );

    if(const auto err = result.left()) {
        return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), {}, {}, {}, {}, { err.value() }, log);
    }
    const auto right = result.right().value();


    if(__flags.verbose) {
        std::cout << "result type: " << right.type() << std::endl;
        std::cout << "result lineage: " << right.lineage() << std::endl;

        std::cout << "--- TREE --- ---" << std::endl;
        wall_e::write_tree(right, std::cout, wall_e::tree_print_format::Simple);
        std::cout << "--- TREE END ---" << std::endl;

        std::cout << "\n -------------- GRAM END --------------\n\n";
    }

    wall_e::write_tree(right, std::cout, wall_e::tree_print_format::Graphviz);

    if(right.contains_type<std::shared_ptr<km2::namespace_node>>()) {
        if(const auto node = right.value<std::shared_ptr<km2::namespace_node>>()) {
            const auto errors = node->errors();

            if(errors.size() > 0) {
                std::cout << wall_e::red << "FOUND ERRORS OF LEVEL 1: " << errors << wall_e::color::reset() << std::endl;
                return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), right, {}, {}, {}, errors, log);
            } else {
                std::cout << wall_e::green << "NO ERRORS OF LEVEL 1" << wall_e::color::reset() << std::endl;
            }

            if(__flags.verbose) {
                std::cout << "AST:" << std::endl;
            }
            node->print(0, std::cout);
            if(__flags.verbose) {
                std::cout << "BACKEND: " << b << std::endl;
            }
            if(!b) {
                return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), node, nullptr, nullptr, {}, log);
            }
            if(__flags.verbose) {
                std::cout << "USING BACKEND: " << b->name() << std::endl;
            }

            const auto unit = b->create_unit();
            if(const auto gen_result = node->generate_backend_value(unit)) {
                backend::value* backend_value = gen_result.right_value();
                unit->print();
                return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), node, unit, backend_value, {}, log);
            } else {
                if(__flags.verbose) {
                    std::cout << wall_e::red << "FOUND ERRORS OF LEVEL 2: " << gen_result.left_value() << wall_e::color::reset() << std::endl;
                }
                return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), node, unit, {}, { gen_result.left_value() }, log);
            }          
        }
    }

    return compilation_result(sorted_tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), {}, {}, {}, { wall_e::error("root node is not a namespace") }, log);
}




const km2::ast_token_list &km2::compilation_result::ast_tokens() const {
    if(m_root_node) {
        if(!m_ast_tokens.has_value()) {
            m_ast_tokens = m_root_node->tokens();
        }
        return *m_ast_tokens;
    } else {
        static const ast_token_list result;
        return result;
    }
}

const wall_e::map<wall_e::text_segment, km2::markup_string> &km2::compilation_result::hovers() const {
    if(m_root_node) {
        if(!m_hovers.has_value()) {
            wall_e::map<wall_e::text_segment, markup_string> result;
            for(const auto& token : ast_tokens()) {
                result.insert({ token.segment, token.hover });
            }
            m_hovers = result;
        }
        return *m_hovers;
    } else {
        static const wall_e::map<wall_e::text_segment, markup_string> result;
        return result;
    }
}

