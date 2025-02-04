#include "flea_ast.hpp"

static std::string id2op(char id) {
  switch (id) {
  case 0:
    return "null";
  case 'l':
    return "<=";
  case 'g':
    return ">=";
  case 'e':
    return "==";
  case 'n':
    return "!=";
  default:
    return std::string(1, id);
  }
}

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
  print_indent(out, indent + AST_INDENT) << *func_type << ", \n";
  print_indent(out, indent + AST_INDENT) << *ident << ", \n";
  block->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
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
  print_indent(out, indent) << "PrimaryExp {\n";
  exp->print(out, indent + AST_INDENT) << '\n';
  return print_indent(out, indent) << "}";
}

std::ostream &NumberAST::print(std::ostream &out, size_t indent) const {
  return print_indent(out, indent) << "Number { " << number << " }";
}

std::ostream &UnaryExpAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << "UnaryExp {\n";
  print_indent(out, indent + AST_INDENT) << id2op(unary_op) << ",\n";
  exp->print(out, indent + AST_INDENT) << "\n";
  return print_indent(out, indent) << "}";
}

std::ostream &BinExpAST::print(std::ostream &out, size_t indent) const {
  print_indent(out, indent) << name() << " {\n";
  print_indent(out, indent + AST_INDENT) << id2op(op) << ",\n";
  lhs->print(out, indent + AST_INDENT);
  if (rhs) {
    rhs->print(out << ",\n", indent + AST_INDENT);
  }
  return print_indent(out << "\n", indent) << "}";
}

std::ostream &operator<<(std::ostream &out, const BaseAST &ast) {
  return ast.print(out);
}