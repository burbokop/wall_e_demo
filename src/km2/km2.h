#ifndef KM2_H
#define KM2_H

#include "error.h"

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>

#include <src/km2/tree/abstract/abstract_node.h>

namespace km2 {

struct compilation_result {
    km2::abstract_node *tree = nullptr;
    std::vector<wall_e::lex::token> tokens;
    std::string rules;
    std::string assembly;
    std::list<km2::error> errors;
};

enum flag {
    only_tree,
    verbose
};

typedef std::list<flag> flags;

compilation_result compile(const std::string &input, const flags &flags = {});

}

#endif // TEST2_H
