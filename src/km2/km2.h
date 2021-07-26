#ifndef KM2_H
#define KM2_H

#include "error.h"

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>
#include <src/km2/utility/asm_tools.h>




struct km2_compilation_result {
    wall_e::variant tree;
    std::vector<wall_e::lex::token> tokens;
    std::string rules;
    wall_e::asm_unit assembly;
    std::list<km2::error> errors;
};

enum km2_flag {
    km2_only_tree,
    km2_verbose
};

typedef std::list<km2_flag> km2_flags;

km2_compilation_result km2_compile(const std::string &input, const km2_flags &flags = km2_flags());


#endif // TEST2_H
