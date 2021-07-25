#include "builder.h"
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
#include <wall_e/src/color.h>

#include <llvm-12/llvm/IR/Value.h>

struct flags_private {
    bool verbose = false;
    bool tree_mode = false;
};

flags_private __km2_parse_flags(const km2_flags &flags) {
    flags_private result;
    for(auto f : flags) {
        if(f == km2_flag::km2_verbose) {
            result.verbose = true;
        } else if(f == km2_flag::km2_only_tree) {
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
    { std::regex("'[^']*'"), "STRING_LITERAL" },
    { std::regex("[a-zA-Z][a-zA-Z0-9]*"), "TOK_ID" },
    { std::regex("[01234567890]+"), "NUMBER" },
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
    { std::regex("[ \t\n]+"), "ignore" }
};



km2_compilation_result km2_compile(const std::string &input, const km2_flags &flags) {
    using namespace wall_e::gram::literals;


    const auto __flags = __km2_parse_flags(flags);
    std::list<km2_error> errors;
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
                std::cout << "[ KLEX ERROR ]\n";
            }

            return { wall_e::variant(), sorted_tokens, std::string(), wall_e::asm_unit(), errors };
        }
    }

    std::list<wall_e::function> functions;

    km2::module_builder builder;

    std::list<wall_e::gram::pattern> gram_list;

    gram_list.push_back("entry << block"_pattern
        << km2::base_node::create);

    gram_list.push_back("block << cmd & SEMICOLON & (0 | block)"_pattern
        << km2::block_node::create);

    gram_list.push_back("internal_block << cmd & SEMICOLON & (EB | internal_block)"_pattern
        << km2::internal_block_node::create);

    gram_list.push_back("cmd << function_call | function_declaration"_pattern
        << km2::cmd_node::create);

    gram_list.push_back("function_declaration << TOK_ID & EQUALS & OP & (EP | decl_arg_list) & OB & (EB | internal_block)"_pattern
        << km2::function_node::create);

    gram_list.push_back("decl_arg_list << decl_arg & (EP | (COMA | decl_arg_list))"_pattern);

    gram_list.push_back("decl_arg << TOK_ID & type"_pattern);

    gram_list.push_back("type << TOK_NUMBER | TOK_STRING"_pattern);

    gram_list.push_back("function_call << TOK_ID & OP & (EP | arg_list)"_pattern
        << km2::call_node::create);

    gram_list.push_back("arg_list << arg & (EP | (COMA & arg_list))"_pattern);

    gram_list.push_back(wall_e::gram::pattern("arg")
                        << (wall_e::gram::rule("TOK_ID") | "STRING_LITERAL" | wall_e::math_patterns::add_to(&gram_list, "math")));

    /*
    gram_list.push_back("asm_insertion << TOK_ASM & OP & STRING_LITERAL & EP"_pattern
                        << [](const wall_e::gram::arg_vector &args) -> wall_e::gram::argument {
        if(args.size() > 2 && args[2].contains_type<wall_e::lex::token>()) {
            const auto token = args[2].value<wall_e::lex::token>();
            if(token.text.size() > 2) {
                auto text = token.text;
                text.erase(text.begin(), text.begin() + 1);
                text.erase(text.end() - 1, text.end());
                text = km2_trim_str(text, '\t');
                text = km2_trim_str(text);
                wall_e::asm_unit result;
                if(text[text.size() - 1] == ':') {
                    result.code = text + "\n";
                } else {
                    result.code = "\t" + text + "\n";
                }
                return result;
            }
            return {};
        }
        return wall_e::gram::pattern::default_processor(args);
    });
    */


    if(__flags.verbose) {
        std::cout << "\n -------------- GRAMATIC --------------\n\n";
    }

    gram_list = wall_e::gram::pattern::simplified(gram_list);

    auto result = wall_e::gram::exec(
                gram_list,
                sorted_tokens,
                gram_flags
                );

    std::cout << "result:" << result << std::endl;


    if(result.contains_type<km2::base_node*>()) {
        if(const auto node = result.value<km2::base_node*>()) {
            std::cout << "node:"<< std::endl;
            node->print(1, std::cout);
        }
    }

    wall_e::asm_unit result_asm_unit;
    result_asm_unit += "\t.globl main\n\n\t.text\nmain:\n";
    result_asm_unit += produce_asm_unit(result);
    result_asm_unit += "\n";

    if(__flags.verbose) {
        std::cout << "\n -------------- GRAM END --------------\n\n";
        std::cout << "\n -------------- ASM CODE --------------\n\n";
        std::cout << result_asm_unit;
        std::cout << "\n -------------- ASM  END --------------\n\n";
    }


    std::ofstream asm_output;

    asm_output.open("./translation.s", std::ios::out);
    if(asm_output.is_open()) {
        asm_output.write(result_asm_unit.code.data(), result_asm_unit.code.size());
        asm_output.close();
        system("gcc -no-pie ./translation.s");
    } else if(__flags.verbose) {
        std::cout << "error: output file not open\n";
    }


    builder.runJit();

    return { result, sorted_tokens, wall_e::gram::pattern::to_string(gram_list), result_asm_unit, errors };
}

std::string km2_trim_str(const std::string &str, char symbol) {
    size_t first = str.find_first_not_of(symbol);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(symbol);
    return str.substr(first, (last - first + 1));
}




bool operator==(const km2_error &error0, const km2_error &error1) {
    return error0.message == error1.message && error0.start == error1.start && error0.end == error1.end;
}


