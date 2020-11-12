#include "km2.h"

#include <regex>
#include <list>
#include <vector>
#include <cassert>
#include <thread>
#include <variant>
#include <iostream>
#include <fstream>

#include <wall_e/src/kgram.h>
#include <wall_e/src/kgram_smp.h>
#include <wall_e/src/tree_view_tools.h>
#include <wall_e/src/lex.h>
#include <wall_e/src/math_patterns.h>
#include <wall_e/src/km2_function.h>
#include <wall_e/src/km2_token_tools.h>


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

const std::list<wall_e::lex::Pattern> km2_lexlist = {
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
    const auto __flags = __km2_parse_flags(flags);
    std::list<km2_error> errors;
    kgram_flags_list gram_flags;
    if(__flags.verbose) {
        gram_flags.push_back(kgram_verbose);
    }
    if(__flags.tree_mode) {
        gram_flags.push_back(kgram_use_default_parser);
    }

    if(__flags.verbose)
        std::cout << ">>> INPUT TEXT <<<\n\n" << input << "\n\n>>> ----- ---- <<<\n";

    const auto tokens = wall_e::lex::get_tokents(input, km2_lexlist);

    if(__flags.verbose) {
        for(auto t : tokens)
            std::cout << t.position << " " << t.name << " " << t.text << "\n";
    }

    const auto sorted_tokens = wall_e::lex::sort_tokens(tokens, input);

    if(__flags.verbose) {
        std::cout << std::endl;
        for(auto t : sorted_tokens)
            std::cout << "sorted: " << t.position << " " << t.name << " " << t.text << "\n";
    }

    for(auto st : sorted_tokens) {
        if(st.name == "error") {
            if(__flags.verbose) {
                std::cout << "[ KLEX ERROR ]\n";
            }

            return { wall_e::variant(), sorted_tokens, std::string(), km2_asm_unit(), errors };
        }
    }

    std::list<km2_function> functions;


    std::list<kgram_pattern_t> gram_list;
    gram_list.push_back(kgram_pattern_t("entry")
                        << kgram_rule_t("block"));

    gram_list.push_back(kgram_pattern_t("block")
                        << ((kgram_rule_t("cmd") & "SEMICOLON") & (kgram_rule_t() | "block")));

    gram_list.push_back(kgram_pattern_t("internal_block")
                        << ((kgram_rule_t("cmd") & "SEMICOLON") & (kgram_rule_t("EB") | "internal_block")));

    gram_list.push_back(kgram_pattern_t("cmd")
                        << (kgram_rule_t("wait") | "curr_time" | "function_call" | "function_declaration" | "asm_insertion"));

    gram_list.push_back(kgram_pattern_t("wait")
                        << (kgram_rule_t("TOK_WAIT") & "NUMBER"));

    gram_list.push_back(kgram_pattern_t("curr_time")
                        << (kgram_rule_t("TOK_CTIME")));

    gram_list.push_back(kgram_pattern_t("function_declaration")
                        << (kgram_rule_t("TOK_ID") & "EQUALS" & "OP" & (kgram_rule_t("EP") | "decl_arg_list") & "OB" & (kgram_rule_t("EB") | "internal_block"))
                        << [&functions](const kgram_arg_vector_t &args) -> kgram_argument_t {
        if(args.size() > 0 && args[0].contains_type<wall_e::lex::Token>()) {
            km2_asm_unit unit;
            const km2_function function(args[0].value<wall_e::lex::Token>().text, km2_produce_token_pairs(args[3]));

            functions.push_back(function);


            unit += "\tjmp __km2_end_" + function.fullName() + "\n";
            unit += "__km2_" + function.fullName() + ":\n";
            unit += "\tpushq %rbp\n";
            unit += "\tmovq %rsp, %rbp\n";

            if(args.size() > 5) {
                unit += km2_produce_asm_unit(args[5]);
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
    gram_list.push_back(kgram_pattern_t("decl_arg_list")
                        << (kgram_rule_t("decl_arg") & (kgram_rule_t("EP") | (kgram_rule_t("COMA") & "decl_arg_list"))));

    gram_list.push_back(kgram_pattern_t("decl_arg")
                        << (kgram_rule_t("type") & "TOK_ID"));

    gram_list.push_back(kgram_pattern_t("type")
                        << (kgram_rule_t("TOK_NUMBER") | "TOK_STRING"));

    gram_list.push_back(kgram_pattern_t("function_call")
                        << (kgram_rule_t("TOK_ID") & "OP" & (kgram_rule_t("EP") | "arg_list"))
                        << [&functions, &errors](const kgram_arg_vector_t &args) -> kgram_argument_t {
        const auto function_name_token = args[0].value<wall_e::lex::Token>();
        const auto function_original_name = function_name_token.text;
        const auto constrained_args = wall_e::variant_constrain(args[2]);
        const auto function_args = km2_remove_tokens(constrained_args, { "EP" });
        const auto overloads = km2_function::find_overloads(function_original_name, functions);

        const auto overload = km2_function::choose_overload(overloads, function_args, [](const std::string& type, const wall_e::variant& value) -> bool {
            if(type == "TOK_NUMBER") {
                return wall_e::is_number(value);
            } else if(type == "TOK_STRING") {
                if(value.contains_type<wall_e::lex::Token>()) {
                    return value.value<wall_e::lex::Token>().name == "STRING_LITERAL";
                }
            }
            return false;
        });

        if(overload.fullName().size() <= 0) {
            errors.push_back({
                                 "function " + function_original_name + " not found",
                                 function_name_token.position,
                                 function_name_token.position + function_original_name.size()
                             });
            return wall_e::variant();
        }
        km2_asm_unit push_asm_unit;
        int stackOffset = 0;
        for(auto argument : function_args) {
            if(wall_e::is_number(argument, "NUMBER")) {
                const uint64_t number = wall_e::to_double(argument);
                push_asm_unit += "\tpushq $" + std::to_string(number) + "\n";
                stackOffset++;
            }
        }

        return push_asm_unit
                + km2_asm_unit { "\tcall __km2_" + overload.fullName() + "\n" }
                + km2_asm_unit { "\tadd $" + std::to_string(stackOffset * 8) + ", %rsp\n" };
    });


    gram_list.push_back(kgram_pattern_t("arg_list")
                        << (kgram_rule_t("arg") & (kgram_rule_t("EP") | (kgram_rule_t("COMA") & "arg_list"))));

    gram_list.push_back(kgram_pattern_t("arg")
                        << (kgram_rule_t("TOK_ID") | "STRING_LITERAL" | math_patterns::add_to(&gram_list, "math")));

    gram_list.push_back(kgram_pattern_t("asm_insertion")
                        << (kgram_rule_t("TOK_ASM") & "OP" & "STRING_LITERAL" & "EP")
                        << [](const kgram_arg_vector_t &args) -> kgram_argument_t {
        if(args.size() > 2 && args[2].contains_type<wall_e::lex::Token>()) {
            const auto token = args[2].value<wall_e::lex::Token>();
            if(token.text.size() > 2) {
                auto text = token.text;
                text.erase(text.begin(), text.begin() + 1);
                text.erase(text.end() - 1, text.end());
                text = km2_trim_str(text, '\t');
                text = km2_trim_str(text);
                km2_asm_unit result;
                if(text[text.size() - 1] == ':') {
                    result.code = text + "\n";
                } else {
                    result.code = "\t" + text + "\n";
                }
                return result;
            }
            return {};
        }
        return kgram_pattern_t::__default_processor(args);
    });




    if(__flags.verbose) {
        std::cout << "\n -------------- GRAMATIC --------------\n\n";
    }

    auto result = kgram_exec(gram_list, sorted_tokens, gram_flags);

    km2_asm_unit result_asm_unit;
    result_asm_unit += "\t.globl main\n\n\t.text\nmain:\n";
    result_asm_unit += km2_produce_asm_unit(result);
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

    return { result, sorted_tokens, kgram_pattern_t::to_string(gram_list), result_asm_unit, errors };
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
