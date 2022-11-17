#include "km2.h"
#include "backend/unit/unit.h"

#include <filesystem>
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
#include "src/km2/tree/stmt_node.h"
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

#include <src/km2/tree/substitution_node.h>

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
        { wall_e::lex::bounded("asm"), "TOK_ASM" },
        { wall_e::lex::bounded("number"), "TOK_NUMBER" },
        { wall_e::lex::bounded("string"), "TOK_STRING" },
        { wall_e::lex::bounded("const"), "TOK_CONST" },
        { wall_e::lex::bounded("(namespace|nspace)"), "TOK_NAMESPACE" },
        { wall_e::lex::bounded("exp"), "TOK_EXP" },
        { wall_e::lex::bounded("imp"), "TOK_IMP" },
        { wall_e::lex::bounded("entry"/** TODO the same as export but choosed as default entry of program (every `exp` can be entry also) */), "TOK_ENTRY" },
        { wall_e::lex::bounded("mut"/** TODO modifier to variable which means variable is mutable */), "TOK_MUT" },
        { wall_e::lex::bounded("irresolute"/** TODO modifier to variable which means variable can be modified everywhere (defualt mut can not be modified after first use in closure) */), "TOK_IRRESOLUTE" },
        { std::regex("u[0-9]+"), "TOK_UNSIGNED" },
        { std::regex("i[0-9]+"), "TOK_SIGNED" },
        { std::regex("f32"), "TOK_FLOAT" },
        { std::regex("f64"), "TOK_DOUBLE" },
        { std::regex("'[^']*'"), "STRING_LITERAL" },
        { wall_e::lex::bounded("_"), "ANONIMUS_ID" },
        { std::regex("[a-zA-Z_][a-zA-Z0-9_]*"), "ID" },
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

km2::compilation_result km2::compile(const backend::backend *b, std::istream &input, const std::string& uri, const flags &flags) {
    return compile(b,
            std::string(
                (std::istreambuf_iterator<char>(input)),
                (std::istreambuf_iterator<char>())
                ),
            uri,
            flags
            );
}

km2::compilation_result km2::compile(const backend::backend* b, const std::string &input, const std::string& uri, const km2::flags &flags) {
    using namespace wall_e::gram::literals;

    if(input.empty()) {
        return compilation_result({}, {}, {}, {}, {}, {}, { wall_e::error("Empty input", wall_e::error::Err, wall_e::error::UndefinedStage, 0) }, {});
    }

    const auto& absolute_uri = uri.empty() ? "" : std::filesystem::absolute(uri).string();

    const auto __flags = __km2_parse_flags(flags);
    wall_e::gram::flags_list gram_flags;
    if(__flags.tree_mode) {
        gram_flags.push_back(wall_e::gram::UseDefaultParser);
    }
    gram_flags.push_back(wall_e::gram::UnconditionalTransition);

    if(__flags.verbose)
        std::cout << ">>> INPUT TEXT <<<\n\n" << input << "\n\n>>> ----- ---- <<<\n";

    const auto tokens = wall_e::lex::make_tokents(input, absolute_uri, km2_lexlist());

    if(__flags.verbose) {
        for(const auto& t : tokens)
            std::cout << t.position << " " << t.name << " " << t.text << "\n";
    }

    wall_e::list<wall_e::error> lex_errors;
    for(const auto& st : tokens) {
        if(const auto& err = st.undef_error()) {
            if(__flags.verbose) {
                std::cout << "[ LEX ERROR ] " << *err << std::endl;
            }
            lex_errors.push_back(*err);
        }
    }

    if(lex_errors.size() > 0) {
        return compilation_result(tokens, {}, {}, {}, {}, {}, lex_errors, {});
    }

    wall_e::gram::pattern_list gram_list;


    const auto& lvalue_fac = lvalue::fac("TOK_EXP", "ID", "ANONIMUS_ID");

    gram_list.push_back("root_block << (0 | root_block_node)"_pattern
        << km2::block_node::create);

    gram_list.push_back("root_block_node << stmt & SEMICOLON & (0 | root_block_node)"_pattern);

    gram_list.push_back("block << OB & (EB | block_node)"_pattern
        << km2::block_node::create);

    gram_list.push_back("block_node << stmt & SEMICOLON & (EB | block_node)"_pattern);

    gram_list.push_back("namespace << TOK_NAMESPACE & block"_pattern
        << km2::namespace_node::create(lvalue_fac));

    gram_list.push_back("lvalue << TOK_EXP | ID | ANONIMUS_ID"_pattern);
    gram_list.push_back("rvalue << namespace | function_declaration | proto_declaration | const"_pattern);

    gram_list.push_back("substitution << lvalue & EQUALS & rvalue"_pattern
                        << km2::substitution_node::create(lvalue_fac));

    //gram_list.push_back("internal_block << (namespace | stmt) & SEMICOLON & (EB | internal_block)"_pattern
    //    << km2::internal_block_node::create);

    gram_list.push_back("stmt << import | function_call | substitution"_pattern
        << km2::stmt_node::create);

    gram_list.push_back("import << TOK_IMP & ID"_pattern
        << km2::imp_node::create);

    gram_list.push_back("const << arg"_pattern
        << km2::const_node::create());

    gram_list.push_back("function_declaration << OP & (EP | decl_arg_list) & block"_pattern
        << km2::function_node::create());

    gram_list.push_back("proto_declaration << OP & (EP | decl_arg_list) & type"_pattern
        << km2::proto_node::create());

    gram_list.push_back("decl_arg_list << decl_arg & (EP | (COMA & decl_arg_list))"_pattern);

    gram_list.push_back("decl_arg << ID & (type | THREE_DOT)"_pattern
        << km2::decl_arg_node::create);

    gram_list.push_back("type << TOK_UNSIGNED | TOK_SIGNED | TOK_FLOAT | TOK_DOUBLE | TOK_STRING"_pattern
        << km2::type_node::create);

    gram_list.push_back("namespace_appeal << ID & DOUBLE_COLON & (namespace_appeal | -)"_pattern);

    gram_list.push_back("function_call << (namespace_appeal | -) & ID & OP & (EP | arg_list)"_pattern
        << km2::call_node::create);

    gram_list.push_back("arg_list << arg & (EP | (COMA & arg_list))"_pattern);

    gram_list.push_back("arg << function_call | substitution | ID | STRING_LITERAL | FLOAT_LITERAL | INT_LITERAL"_pattern
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

    environment env(absolute_uri, flags);

    auto result = wall_e::gram::exec(
                gram_list,
                tokens,
                &env,
                gram_flags, wall_e::smp::simplify,
                log.get()
                );

    if(const auto err = result.left()) {
        return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), {}, {}, {}, {}, { err.value() }, log);
    }
    const auto right = result.right().value();


    if(__flags.verbose) {
        std::cout << "gram result type: " << right.type() << std::endl;
        std::cout << "gram result lineage: " << right.lineage() << std::endl;

        if(__flags.tree_mode) {
            std::cout << "--- TOKEN TREE --- ---" << std::endl;
            wall_e::write_tree(right, std::cout, wall_e::tree_print_format::Simple);
            std::cout << "--- TOKEN TREE GRAPHVIZ ---" << std::endl;
            wall_e::write_tree(right, std::cout, wall_e::tree_print_format::Graphviz);
            std::cout << "--- TOKEN TREE END ---" << std::endl;
        }

        std::cout << "\n -------------- GRAM END --------------\n\n";
    }

    if(const auto& root_node = right.option_cast<std::shared_ptr<km2::abstract_value_node>>()) {
        const auto errors = (*root_node)->errors();

        if(errors.size() > 0) {
            if(__flags.verbose) {
                std::cout << wall_e::red << "FOUND ERRORS OF LEVEL 1: " << errors << wall_e::color::reset() << std::endl;
            }
            return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), right, {}, {}, {}, errors, log);
        } else {
            if(__flags.verbose) {
                std::cout << wall_e::green << "NO ERRORS OF LEVEL 1" << wall_e::color::reset() << std::endl;
            }
        }

        std::cout << "root node: " << (*root_node) << std::endl;

        if(__flags.verbose) {
            std::cout << "AST --------" << std::endl;
            (*root_node)->write(std::cout, abstract_node::Simple, wall_e::tree_writer::context::detached());
            std::cout << "AST GRAPHVIZ" << std::endl;

            wall_e::graphviz_tree_writer writer;
            (*root_node)->write(std::cout, abstract_node::TreeWriter, writer.root());
            std::cout << "AST END ----" << std::endl;
        }
        if(__flags.verbose) {
            std::cout << "BACKEND: " << b << std::endl;
        }
        if(!b) {
            return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), *root_node, nullptr, nullptr, {}, log);
        }
        if(__flags.verbose) {
            std::cout << "USING BACKEND: " << b->name() << std::endl;
        }

        const auto unit = b->create_unit();
        if(const auto gen_result = (*root_node)->generate_backend_value(unit)) {
            backend::value* backend_value = gen_result.right_value();
            unit->print();
            return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), *root_node, unit, backend_value, {}, log);
        } else {
            if(__flags.verbose) {
                std::cout << wall_e::red << "FOUND ERRORS OF LEVEL 2: " << gen_result.left_value() << wall_e::color::reset() << std::endl;
            }
            return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), *root_node, unit, {}, { gen_result.left_value() }, log);
        }
    }
    return compilation_result(tokens, wall_e::gram::pattern::to_string(gram_list), result.right().value(), {}, {}, {}, { wall_e::error("root node not a value node") }, log);
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

