#include "clang_namespace_node.h"

#include <iostream>

#include "buffered_reverse_range.h"
#include "clang_function_node.h"
#include "tools.h"

#include <src/km2/tree/function_node.h>

clang_namespace_node::target_ptr_type clang_namespace_node::km2_node() const {
    std::shared_ptr<km2::block_node> current_block_node;

    for(const auto& d : buffered_reverse_range(origin->decls())) {

        if(const clang_function_node& function_node = dynamic_cast<clang::FunctionDecl*>(d)) {
            const auto& stmt = std::make_shared<km2::stmt_node>(function_node.km2_node());
            current_block_node = std::make_shared<km2::block_node>(stmt, current_block_node);
        }

        std::cout << "decl_name: " << clang_tools::decl_name(d) << std::endl;
    }

    return std::make_shared<km2::namespace_node>(origin->getNameAsString(), current_block_node);
}
