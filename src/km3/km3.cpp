#include "km3.h"

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
#include <wall_e/src/utility/math_patterns.h>
#include <wall_e/src/utility/function.h>
#include <wall_e/src/utility/token_tools.h>


const std::list<wall_e::lex::pattern> PCDProcessor::lex = {
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


PCDProcessor::Unit PCDProcessor::compile(const std::string &input) {
    const auto tokens = wall_e::lex::parse(input, lex);
    std::list<Error> errors;

    std::list<wall_e::function> functions;

    std::list<wall_e::gram::kgram_pattern_t> gram_list;
    gram_list.push_back(wall_e::gram::kgram_pattern_t("entry")
                        << wall_e::gram::kgram_rule_t("block"));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("block")
                        << ((wall_e::gram::kgram_rule_t("cmd") & "SEMICOLON") & (wall_e::gram::kgram_rule_t() | "block")));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("internal_block")
                        << ((wall_e::gram::kgram_rule_t("cmd") & "SEMICOLON") & (wall_e::gram::kgram_rule_t("EB") | "internal_block")));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("cmd")
                        << (wall_e::gram::kgram_rule_t("wait") | "curr_time" | "function_call" | "function_declaration" | "asm_insertion"));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("wait")
                        << (wall_e::gram::kgram_rule_t("TOK_WAIT") & "NUMBER"));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("curr_time")
                        << (wall_e::gram::kgram_rule_t("TOK_CTIME")));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("function_declaration")
                        << (wall_e::gram::kgram_rule_t("TOK_ID") & "EQUALS" & "OP" & (wall_e::gram::kgram_rule_t("EP") | "decl_arg_list") & "OB" & (wall_e::gram::kgram_rule_t("EB") | "internal_block"))
                        << [&functions](const wall_e::gram::kgram_arg_vector_t &args) -> wall_e::gram::kgram_argument_t {
        if(args.size() > 0 && args[0].contains_type<wall_e::lex::token>()) {
            wall_e::asm_unit unit;
            const wall_e::function function(args[0].value<wall_e::lex::token>().text, produce_token_pairs(args[3]));

            functions.push_back(function);


            unit += "\tjmp __km2_end_" + function.fullName() + "\n";
            unit += "__km2_" + function.fullName() + ":\n";
            unit += "\tpushq %rbp\n";
            unit += "\tmovq %rsp, %rbp\n";

            if(args.size() > 5) {
                unit += produce_asm_unit(args[5]);
            }

            unit += "\tmovq %rbp, %rsp\n";
            unit += "\tpopq %rbp\n";
            unit += "\tret\n";
            unit += "__km2_end_" + function.fullName() + ":\n";
            return unit;
        }
        return args;
    });


    //https://www.onlinegdb.com/online_gcc_assembler
    gram_list.push_back(wall_e::gram::kgram_pattern_t("decl_arg_list")
                        << (wall_e::gram::kgram_rule_t("decl_arg") & (wall_e::gram::kgram_rule_t("EP") | (wall_e::gram::kgram_rule_t("COMA") & "decl_arg_list"))));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("decl_arg")
                        << (wall_e::gram::kgram_rule_t("type") & "TOK_ID"));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("type")
                        << (wall_e::gram::kgram_rule_t("TOK_NUMBER") | "TOK_STRING"));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("function_call")
                        << (wall_e::gram::kgram_rule_t("TOK_ID") & "OP" & (wall_e::gram::kgram_rule_t("EP") | "arg_list"))
                        << [&functions, &errors](const wall_e::gram::kgram_arg_vector_t &args) -> wall_e::gram::kgram_argument_t {
        const auto function_name_token = args[0].value<wall_e::lex::token>();
        const auto function_original_name = function_name_token.text;
        const auto constrained_args = args[2].constrain();
        const auto function_args = remove_tokens(constrained_args, { "EP" });
        const auto overloads = wall_e::function::find_overloads(function_original_name, functions);


        const auto overload = wall_e::function::choose_overload(overloads, function_args, [](const std::string& type, const wall_e::variant& value) -> bool {
            if(type == "TOK_NUMBER") {
                return wall_e::is_number(value);
            } else if(type == "TOK_STRING") {
                if(value.contains_type<wall_e::lex::token>()) {
                    return value.value<wall_e::lex::token>().name == "STRING_LITERAL";
                }
            }
            return false;
        });


        if(overload.fullName().size() <= 0) {
            //errors.push_back({
            //                     "function " + function_original_name + " not found",
            //                     function_name_token.position,
            //                     function_name_token.position + function_original_name.size()
            //                 });
            return wall_e::variant();
        }
        wall_e::asm_unit push_asm_unit;
        int stackOffset = 0;
        for(auto argument : function_args) {
            if(wall_e::is_number(argument, "NUMBER")) {
                const uint64_t number = wall_e::to_double(argument);
                push_asm_unit += "\tpushq $" + std::to_string(number) + "\n";
                stackOffset++;
            }
        }

        return push_asm_unit
                + wall_e::asm_unit { "\tcall __km2_" + overload.fullName() + "\n" }
                + wall_e::asm_unit { "\tadd $" + std::to_string(stackOffset * 8) + ", %rsp\n" };
    });


    gram_list.push_back(wall_e::gram::kgram_pattern_t("arg_list")
                        << (wall_e::gram::kgram_rule_t("arg") & (wall_e::gram::kgram_rule_t("EP") | (wall_e::gram::kgram_rule_t("COMA") & "arg_list"))));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("arg")
                        << (wall_e::gram::kgram_rule_t("TOK_ID") | "STRING_LITERAL" | wall_e::math_patterns::add_to(&gram_list, "math")));

    gram_list.push_back(wall_e::gram::kgram_pattern_t("asm_insertion")
                        << (wall_e::gram::kgram_rule_t("TOK_ASM") & "OP" & "STRING_LITERAL" & "EP")
                        << [](const wall_e::gram::kgram_arg_vector_t &args) -> wall_e::gram::kgram_argument_t {
        if(args.size() > 2 && args[2].contains_type<wall_e::lex::token>()) {
            const auto token = args[2].value<wall_e::lex::token>();
            if(token.text.size() > 2) {
                auto text = token.text;
                text.erase(text.begin(), text.begin() + 1);
                text.erase(text.end() - 1, text.end());
                //text = km2_trim_str(text, '\t');
                //text = km2_trim_str(text);
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
        return wall_e::gram::kgram_pattern_t::__default_processor(args);
    });




    //if(__flags.verbose) {
    //    std::cout << "\n -------------- GRAMATIC --------------\n\n";
    //}

    //auto result = kgram_exec(gram_list, sorted_tokens, gram_flags);

    wall_e::asm_unit result_asm_unit;
    result_asm_unit += "\t.globl main\n\n\t.text\nmain:\n";
    //result_asm_unit += produce_asm_unit(result);
    result_asm_unit += "\n";

    //if(__flags.verbose) {
    //    std::cout << "\n -------------- GRAM END --------------\n\n";
    //    std::cout << "\n -------------- ASM CODE --------------\n\n";
    //    std::cout << result_asm_unit;
    //    std::cout << "\n -------------- ASM  END --------------\n\n";
    //}


    std::ofstream asm_output;

    //asm_output.open("./translation.s", std::ios::out);
    //if(asm_output.is_open()) {
    //    asm_output.write(result_asm_unit.code.data(), result_asm_unit.code.size());
    //    asm_output.close();
    //    system("gcc -no-pie ./translation.s");
    //} else if(__flags.verbose) {
    //    std::cout << "error: output file not open\n";
    //}

    //return { result, sorted_tokens, wall_e::gram::kgram_pattern_t::to_string(gram_list), result_asm_unit, errors };
    return {};
}

std::string trimString(const std::string &str, char symbol) {
    size_t first = str.find_first_not_of(symbol);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(symbol);
    return str.substr(first, (last - first + 1));
}
