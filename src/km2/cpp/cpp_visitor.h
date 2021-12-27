#ifndef CPP_VISITOR_H
#define CPP_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>

#include <memory>

#include <src/km2/tree/namespace_node.h>



class cpp_visitor : public clang::RecursiveASTVisitor<cpp_visitor> {
  clang::SourceManager &m_source_manager;

  std::shared_ptr<km2::namespace_node> m_result_node;

public:
  cpp_visitor(clang::SourceManager &source_manager);

  bool VisitNamedDecl(clang::NamedDecl *named_decl);

  std::shared_ptr<km2::namespace_node> result_node() const;

private:
  std::string getDeclLocation(clang::SourceLocation loc) const;
};

#endif // CPP_VISITOR_H
