#include "cpp_base_node.h"

#include <iostream>

cpp_base_node::cpp_base_node(clang::TranslationUnitDecl* decl) {
    llvm::outs() << "AAAAAAA ORIGINAL " << decl << "\n";


    //decl->decls

    //clang::Decl::Kind::

    std::cout << "getKind: " << dynamic_cast<clang::Decl*>(decl)->getDeclKindName() << std::endl;


    const auto&& declName = [](clang::Decl* decl) -> std::string {
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
    };


    for(const auto& d : decl->decls()) {
        std::cout << "\tgetKind: " << dynamic_cast<clang::Decl*>(d)->getDeclKindName() << " " << declName(d) << ": ";

        if(const auto& td = dynamic_cast<clang::TypedefDecl*>(d)) {
            std::cout << td->getName().str();
        } else if(const auto& td = dynamic_cast<clang::NamespaceDecl*>(d)) {
            std::cout << td->getName().str();
        }
        std::cout << std::endl;
    }

    if(const auto& nspace = decl->getAnonymousNamespace()) {
        if(nspace->isOriginalNamespace()) {
            llvm::outs() << "IS ORIGINAL\n";
        } else {
            llvm::outs() << "NOT AN ORIGINAL\n";
        }
    } else {
        llvm::outs() << "EMPTY NAMESPACE" << "\n";
    }


    //nspace.getMostRecentDecl()

}
