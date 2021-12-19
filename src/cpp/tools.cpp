#include "tools.h"

std::string clang_tools::decl_name(clang::Decl *decl) {
    if(const auto& td = dynamic_cast<clang::NamedDecl*>(decl)) {
        const auto dn = td->getDeclName();
        if(dn.isIdentifier()) {
            const auto& name = td->getName().str();
            if(name.empty()) {
                return "[empty str]";
            } else {
                return std::string("'") + name + "'";
            }
        }
        return "[not identifier]";
    }
    return "[not named]";
}
