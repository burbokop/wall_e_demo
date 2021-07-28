#ifndef KM2_H
#define KM2_H

#include "error.h"

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>

#include <src/km2/tree/abstract/abstract_node.h>

namespace llvm {
class Value;
}

namespace km2 {
class module;


struct compilation_result {
    wall_e::variant token_tree;
    std::shared_ptr<km2::abstract_node> root_node;
    std::vector<wall_e::lex::token> tokens;
    std::string rules;
    std::shared_ptr<module> mod;
    llvm::Value* llvm_value;
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
