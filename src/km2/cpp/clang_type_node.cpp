#include "clang_type_node.h"
#include "src/km2/cpp/tools.h"


clang_type_node::target_ptr_type clang_type_node::km2_node() const {

    if (const auto *builtinType = origin->getAs<clang::BuiltinType>()) {
        switch (builtinType->getKind()) {
            case clang::BuiltinType::Kind::Int: return std::make_shared<km2::type_node>(clang_tools::index_plug(), km2::type_node::Signed, std::optional(32));
            case clang::BuiltinType::Kind::UInt: return std::make_shared<km2::type_node>(clang_tools::index_plug(), km2::type_node::Unsigned, std::optional(32));
            default: return std::make_shared<km2::type_node>(clang_tools::index_plug(), km2::type_node::Undefined, std::nullopt);
        }
    }
    return std::make_shared<km2::type_node>(clang_tools::index_plug(), km2::type_node::Undefined, std::nullopt);
}
