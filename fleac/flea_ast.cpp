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

void CompUnitAST::print(std::ostream &out) const { func_def->print(out); }

void FuncDefAST::print(std::ostream &out) const {
  out << *func_type << ' ' << *ident << "() ";
  block->print(out);
}

void BlockAST::print(std::ostream &out) const {
  out << "{ ";
  stmt->print(out);
  out << " }";
}

void StmtAST::print(std::ostream &out) const {
  out << "return ";
  exp->print(out);
  out << ";";
}

void ExpAST::print(std::ostream &out) const { unary_exp->print(out); }

void PrimaryExpAST::print(std::ostream &out) const {
  out << "(";
  exp->print(out);
  out << ")";
}

void NumberAST::print(std::ostream &out) const { out << number; }

void UnaryExpAST::print(std::ostream &out) const {
  if (unary_op)
    out << id2op(unary_op);
  exp->print(out);
}

void BinExpAST::print(std::ostream &out) const {
  lhs->print(out);
  if (rhs)
    rhs->print(out << id2op(op));
}

std::ostream &operator<<(std::ostream &out, const BaseAST &ast) {
  return ast.print(out), out;
}