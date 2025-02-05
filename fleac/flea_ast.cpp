#include "flea_ast.hpp"

static constexpr std::string id2tp(char id) {
  switch (id) {
  case 0:
  case 'v':
    return "void";
  case 1:
  case 'i':
    return "int";
  default:
    __builtin_unreachable();
  }
}

static constexpr std::string id2op(char id) {
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
  out << id2tp(func_type_id) << ' ' << *ident << "() ";
  block->print(out);
}

void BlockAST::print(std::ostream &out) const {
  out << "{ ";
  for (const auto &item : *item_l)
    item->print(out), out << ' ';
  out << "}";
}

void BlockItemAST::print(std::ostream &out) const { item->print(out); }

void DeclAST::print(std::ostream &out) const { decl->print(out); }

void ConstDeclAST::print(std::ostream &out) const {
  out << "const " << id2tp(type_id) << ' ';
  const auto &def_l = *this->def_l;
  for (size_t i = 0; i < def_l.size(); ++i) {
    if (i != 0)
      out << ", ";
    def_l[i]->print(out);
  }
  out << ";";
}

void ConstDefAST::print(std::ostream &out) const {
  out << *ident << " = ";
  const_init_val->print(out);
}

void ConstInitValAST::print(std::ostream &out) const { const_exp->print(out); }

void StmtAST::print(std::ostream &out) const {
  out << "return ";
  exp->print(out);
  out << ";";
}

void ExpAST::print(std::ostream &out) const { unary_exp->print(out); }

void ConstExpAST::print(std::ostream &out) const { exp->print(out); }

void PrimaryExpAST::print(std::ostream &out) const {
  out << "(";
  exp->print(out);
  out << ")";
}

void LValAST::print(std::ostream &out) const { out << *ident; }

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