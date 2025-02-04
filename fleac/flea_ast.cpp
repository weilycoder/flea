#include "flea_ast.hpp"

static std::ostream &print_indent(std::ostream &out, size_t indent) {
  for (size_t i = 0; i < indent; ++i)
    out.put(' ');
  return out;
}

std::ostream &CompUnitAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "CompUnit {\n";
  func_def->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &FuncDefAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "FuncDef {\n";
  func_type->print(out, indent + AST_INDENT) << ", \n";
  print_indent(out, indent + AST_INDENT) << ident << ", \n";
  block->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &FuncTypeAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "FuncType { " << ident << " }";
}

std::ostream &BlockAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "Block {\n";
  stmt->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &StmtAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "Stmt {\n";
  exp->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &ExpAST::print(std::ostream &out, size_t indent) const {
  return unary_exp->print(out, indent);
}

std::ostream &PrimaryExpAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "PrimaryExp { " << *exp << " }";
}

std::ostream &NumberAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "Number { " << number << " }";
}

std::ostream &UnaryExpAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "UnaryExp { ";
  if (!unary_op)
    out << "null, ";
  else
    out << unary_op << ", ";
  return exp->print(out) << " }";
}

std::ostream &BinExpAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << name() << " { ";
  if (!op)
    out << "null, ";
  else
    out << op << ", ";
  out << *lhs;
  if (rhs)
    out << ", " << *rhs;
  return out << " }";
}

std::ostream &operator<<(std::ostream &out, const BaseAST &ast) {
  return ast.print(out);
}