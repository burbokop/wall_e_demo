#ifndef TOOLS_H
#define TOOLS_H


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <clang/AST/Decl.h>
#pragma GCC diagnostic pop

namespace clang_tools {
std::string decl_name(clang::Decl* decl);
}


#endif // TOOLS_H
