#include "cpp_visitor.h"

#include <sstream>
#include <clang/Basic/SourceManager.h>

std::shared_ptr<km2::namespace_node> cpp_visitor::result_node() const {
    return m_result_node;
}

cpp_visitor::cpp_visitor(clang::SourceManager &source_manager)
    : m_source_manager(source_manager) {}

bool cpp_visitor::VisitNamedDecl(clang::NamedDecl *named_decl) {

    llvm::outs() << "Found " << named_decl->getQualifiedNameAsString() << " at "
                 << getDeclLocation(named_decl->getLocation()) << "\n";
    return true;
}

std::string cpp_visitor::getDeclLocation(clang::SourceLocation loc) const {
    std::ostringstream oss;
    oss << m_source_manager.getFilename(loc).str() << ":"
        << m_source_manager.getSpellingLineNumber(loc) << ":"
        << m_source_manager.getSpellingColumnNumber(loc);
    return oss.str();
}
