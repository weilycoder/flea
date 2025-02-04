#include "flea_ast.hpp"

std::ostream &print_indent(std::ostream &out, size_t indent) {
  for (size_t i = 0; i < indent; ++i)
    out.put(' ');
  return out;
}

std::ostream &CompUnitAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "CompUnit { " << '\n';
  func_def->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &FuncDefAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "FuncDef { \n";
  func_type->print(out, indent + AST_INDENT) << ", \n";
  print_indent(out, indent + AST_INDENT) << ident << ", \n";
  block->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &FuncTypeAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "FuncType { " << ident << " }";
}

std::ostream &BlockAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "Block { ";
  return stmt->print(out) << " }";
}

std::ostream &StmtAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "Stmt { " << number << " }";
}

std::ostream &operator<<(std::ostream &out, const BaseAST &ast) {
  return ast.print(out);
}