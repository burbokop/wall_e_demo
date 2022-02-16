#ifndef KM2_H
#define KM2_H

#include "error.h"

#include <string>


#if __has_include(<wall_e/src/models/variant.h>)
    #include <wall_e/src/models/variant.h>
#else
    #include <wall_e/models/variant.h>
#endif

#if __has_include(<wall_e/src/lex.h>)
    #include <wall_e/src/lex.h>
#else
    #include <wall_e/lex.h>
#endif

namespace llvm {
class Value;
}

namespace km2 {

class translation_unit;
class abstract_node;

struct compilation_result {
    wall_e::variant token_tree;
    std::shared_ptr<km2::abstract_node> root_node;
    std::vector<wall_e::lex::token> tokens;
    std::string rules;
    std::shared_ptr<translation_unit> unit;
    llvm::Value* llvm_value;
    std::list<wall_e::error> errors;
    std::map<wall_e::text_segment, std::string> hovers;
};

enum flag {
    only_tree,
    verbose,
    generate_lsp_info
};

typedef std::list<flag> flags;

std::list<std::string> key_names();

compilation_result compile(const std::string &input, const flags &flags = {});

}

#endif // TEST2_H
