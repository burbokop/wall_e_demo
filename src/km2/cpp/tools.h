#ifndef TOOLS_H
#define TOOLS_H


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <clang/AST/Decl.h>
#pragma GCC diagnostic pop

#include <wall_e/src/models/index.h>

namespace clang_tools {
std::string decl_name(clang::Decl* decl);

[[deprecated("Do not use it")]]
inline wall_e::index index_plug() { return wall_e::index(0, 0, 0, 0); }
}


#endif // TOOLS_H
