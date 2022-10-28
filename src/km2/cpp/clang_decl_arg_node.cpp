#include "clang_decl_arg_node.h"
#include "clang_type_node.h"
#include "src/km2/cpp/tools.h"


clang_decl_arg_node::target_ptr_type clang_decl_arg_node::km2_node() const {
    clang_type_node node = origin->getType().getTypePtr();

    return std::make_shared<km2::decl_arg_node>(clang_tools::index_plug(), origin->getNameAsString(), node ? node.km2_node() : nullptr, false);
}
