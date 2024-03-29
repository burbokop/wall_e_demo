#include "clang_block_node.h"
#include "clang_decl_arg_node.h"
#include "clang_function_node.h"
#include "src/km2/cpp/tools.h"

#include <clang/AST/Stmt.h>

clang_function_node::target_ptr_type clang_function_node::km2_node() const {

    //decl.block
    //origin->getBody()->dumpPretty(origin->getASTContext());

    wall_e::vec<std::shared_ptr<km2::decl_arg_node>> args;
    for(const auto& p : origin->parameters()) {
        if(const clang_decl_arg_node& node = p) {
            args.push_back(node.km2_node());
        } else {
            args.push_back(nullptr);
        }
    }

    return std::make_shared<km2::function_node>(
                clang_tools::index_plug(),
                origin->getNameAsString(),
                wall_e::text_segment(),
                args,
                nullptr//std::make_shared<clang_block_node>(decl->getBody())
                );
}
