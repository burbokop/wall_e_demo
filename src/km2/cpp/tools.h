#ifndef TOOLS_H
#define TOOLS_H

#include <clang/AST/Decl.h>

namespace clang_tools {
std::string decl_name(clang::Decl* decl);
}


#endif // TOOLS_H
