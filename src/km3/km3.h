#ifndef PCDPROCESSOR_H
#define PCDPROCESSOR_H

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>
#include <wall_e/src/utility/asm_tools.h>

/**
 * @brief The PCDProcessor class
 *
 * @details
 * cmds:
 *  call
 *  alloc-mem
 *  deloc-mem
 *  plus
 *  minus
 *  mul
 *  div
 */


class PCDProcessor {
public:
    static std::string trimString(const std::string& str, char symbol = ' ');
    static const std::list<wall_e::lex::pattern> lex;
    typedef std::pair<size_t, std::string> Error;

    struct Call {};

    struct Cmd {
        enum Type {
            Call,
            AllocMem,
            DelocMem,
            Plus,
            Minus,
            Mul,
            Div,
            Asm
        };
        Type type;
    };

    struct Function {
        std::list<Cmd> commands;
    };

    struct Unit {
        std::list<Cmd> commands;
    };

    struct Result {
        Unit unit;

        wall_e::variant tree;
        std::vector<wall_e::lex::token> tokens;
        std::string rules;
        std::list<Error> errors;
    };

    Unit compile(const std::string &input);

};

#endif // PCDPROCESSOR_H
