#include "module.h"
#include "km2.h"

#include <regex>
#include <list>
#include <vector>
#include <cassert>
#include <thread>
#include <variant>
#include <iostream>
#include <fstream>

#include <wall_e/src/gram.h>
#include <wall_e/src/utility/tree_view_tools.h>
#include <wall_e/src/lex.h>
#include <src/km2/utility/math_patterns.h>
#include <src/km2/utility/function.h>
#include <wall_e/src/utility/token_tools.h>
#include <wall_e/src/private/gram_smp.h>
#include <src/km2/tree/base_node.h>
#include <src/km2/tree/call_node.h>
#include <src/km2/tree/function_node.h>
#include <src/km2/tree/internal_block_node.h>
#include <src/km2/tree/arg_node.h>
#include <src/km2/tree/proto_node.h>
#include <src/km2/tree/type_node.h>
#include <src/km2/tree/decl_arg_node.h>
#include <wall_e/src/color.h>

#include <llvm-12/llvm/IR/Value.h>

struct __km2_flags_private {
    bool verbose = false;
    bool tree_mode = false;
};

__km2_flags_private __km2_parse_flags(const km2::flags &flags) {
    __km2_flags_private result;
    for(auto f : flags) {
        if(f == km2::flag::verbose) {
            result.verbose = true;
        } else if(f == km2::flag::only_tree) {
            result.tree_mode = true;
        }
    }
    return result;
}

const std::list<wall_e::lex::pattern> km2_lexlist = {
    { std::regex("wait[ \t\n]"), "TOK_WAIT" },
    { std::regex("asm"), "TOK_ASM" },
    { std::regex("number"), "TOK_NUMBER" },
    { std::regex("string"), "TOK_STRING" },
    { std::regex("current_time[(][)]"), "TOK_CTIME" },
    { std::regex("u[0-9]+"), "TOK_UNSIGNED" },
    { std::regex("i[0-9]+"), "TOK_SIGNED" },
    { std::regex("f32"), "TOK_FLOAT" },
    { std::regex("f64"), "TOK_DOUBLE" },
    { std::regex("'[^']*'"), "STRING_LITERAL" },
    { std::regex("[a-zA-Z][a-zA-Z0-9]*"), "TOK_ID" },
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
    { std::regex(":"), "COLON" },
    { std::regex(","), "COMA" },
    { std::regex("\\.\\.\\."), "THREE_DOT" },

    { std::regex("[ \t\n]+"), "ignore" }
};


void aaa(std::string && a) {
    a.resize(10);
    std::cout << __PRETTY_FUNCTION__ << ": " << a << "\n";
}

void aaa(const std::string &a) {
    auto copy = a;
    copy.resize(10);
    std::cout << __PRETTY_FUNCTION__ << ": " << a << "\n";
}

km2::compilation_result km2::compile(const std::string &input, const km2::flags &flags) {

    std::string a = "const&";

    aaa(a);
    aaa("tmp");

    using namespace wall_e::gram::literals;


    const auto __flags = __km2_parse_flags(flags);
    wall_e::gram::flags_list gram_flags;
    if(__flags.verbose) {
        gram_flags.push_back(wall_e::gram::verbose);
    }
    if(__flags.tree_mode) {
        gram_flags.push_back(wall_e::gram::use_default_parser);
    }
    gram_flags.push_back(wall_e::gram::unconditional_transition);

    if(__flags.verbose)
        std::cout << ">>> INPUT TEXT <<<\n\n" << input << "\n\n>>> ----- ---- <<<\n";

    const auto tokens = wall_e::lex::make_tokents(input, km2_lexlist);

    if(__flags.verbose) {
        for(const auto& t : tokens)
            std::cout << t.position << " " << t.name << " " << t.text << "\n";
    }

    const auto sorted_tokens = wall_e::lex::sort_tokens(tokens, input);

    if(__flags.verbose) {
        std::cout << std::endl;
        for(const auto& t : sorted_tokens)
            std::cout << "sorted: " << t.position << " " << t.name << " " << t.text << "\n";
    }

    for(const auto& st : sorted_tokens) {
        if(st.name == "error") {
            if(__flags.verbose) {
                std::cout << "[ LEX ERROR ]\n";
            }

            return { {}, nullptr, sorted_tokens, std::string(), {}, {} };
        }
    }

    std::list<wall_e::function> functions;

    std::list<wall_e::gram::pattern> gram_list;

    gram_list.push_back("entry << block"_pattern
        << km2::base_node::create);

    gram_list.push_back("block << cmd & SEMICOLON & (0 | block)"_pattern
        << km2::block_node::create);

    gram_list.push_back("internal_block << cmd & SEMICOLON & (EB | internal_block)"_pattern
        << km2::internal_block_node::create);

    gram_list.push_back("cmd << function_call | function_declaration | proto_declaration"_pattern
        << km2::cmd_node::create);

    gram_list.push_back("function_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & OB & (EB | internal_block)"_pattern
        << km2::function_node::create);

    gram_list.push_back("proto_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & type"_pattern
        << km2::proto_node::create);

    gram_list.push_back("decl_arg_list << decl_arg & (EP | (COMA & decl_arg_list))"_pattern);

    gram_list.push_back("decl_arg << TOK_ID & (type | THREE_DOT)"_pattern
        << km2::decl_arg_node::create);

    gram_list.push_back("type << TOK_UNSIGNED | TOK_SIGNED | TOK_FLOAT | TOK_DOUBLE | TOK_STRING"_pattern
        << km2::type_node::create);

    gram_list.push_back("function_call << TOK_ID & OP & (EP | arg_list)"_pattern
        << km2::call_node::create);

    gram_list.push_back("arg_list << arg & (EP | (COMA & arg_list))"_pattern);

    gram_list.push_back("arg << function_call | TOK_ID | STRING_LITERAL | FLOAT_LITERAL | INT_LITERAL"_pattern
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
    }

    gram_list = wall_e::gram::pattern::simplified(gram_list);

    auto result = wall_e::gram::exec(
                gram_list,
                sorted_tokens,
                gram_flags
                );

    std::cout << "result: " << result << std::endl;
    std::cout << "result type: " << result.type() << std::endl;
    std::cout << "result lineage: " << result.lineage() << std::endl;

    if(__flags.verbose) {
        std::cout << "\n -------------- GRAM END --------------\n\n";
    }


    if(result.contains_type<std::shared_ptr<km2::base_node>>()) {
        if(const auto node = result.value<std::shared_ptr<km2::base_node>>()) {
            const auto errors = node->errors();

            if(errors.size() > 0) {
                std::cout << wall_e::color::Red << "FOUND ERRORS OF LEVEL 1: " << errors << wall_e::color::reset() << std::endl;
            } else {
                std::cout << wall_e::color::Green << "NO ERRORS OF LEVEL 1" << wall_e::color::reset() << std::endl;
            }

            std::cout << "AST:" << std::endl;
            node->print(0, std::cout);


            std::cout << "LLVM:"<< std::endl;
            const auto module = std::make_shared<km2::module>();            
            if(const auto gen_result = node->generate_llvm(module)) {
                llvm::Value* llvm_value = gen_result.right_value();

                module->print();

                return {
                    .token_tree = result,
                    .root_node = node,
                    .tokens = sorted_tokens,
                    .rules = wall_e::gram::pattern::to_string(gram_list),
                    .mod = module,
                    .llvm_value = llvm_value,
                    .errors = {}
                };
            } else {
                std::cout << wall_e::color::Red << "FOUND ERRORS OF LEVEL 2: " << gen_result.left_value() << wall_e::color::reset() << std::endl;
                return { result, node, sorted_tokens, wall_e::gram::pattern::to_string(gram_list), {}, {}, { gen_result.left_value() } };
            }          
        }
    }

    return { result, nullptr, sorted_tokens, wall_e::gram::pattern::to_string(gram_list), {}, {}};
}


