#include "flea_ast.hpp"
#include "flea_com.hpp"
#include <cassert>

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

// print functions

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

void DeclAST::print(std::ostream &out) const {
  if (is_const)
    out << "const ";
  out << id2tp(type_id) << ' ';
  const auto &def_l = *this->def_l;
  for (size_t i = 0; i < def_l.size(); ++i) {
    if (i != 0)
      out << ", ";
    def_l[i]->print(out);
  }
  out << ";";
}

void DefAST::print(std::ostream &out) const {
  out << *ident << " = ";
  const_init_val->print(out);
}

void InitValAST::print(std::ostream &out) const { const_exp->print(out); }

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

// const_eval functions

int64_t fold_const(std::unique_ptr<BaseAST> &ast, SymbolTable *stb,
                   bool force) {
  int64_t val = ast->const_eval(stb, force);
  if (force)
    assert(val != INT64_MAX);
  if (val == INT64_MAX)
    return INT64_MAX;
  ast.reset(new NumberAST((int32_t)val));
  return val;
}

int64_t CompUnitAST::const_eval(SymbolTable *stb, bool force) {
  return func_def->const_eval(stb, force);
}

int64_t FuncDefAST::const_eval(SymbolTable *stb, bool force) {
  return block->const_eval(stb, force);
}

int64_t BlockAST::const_eval(SymbolTable *stb, bool force) {
  for (const auto &item : *item_l)
    item->const_eval(stb, force);
  return INT64_MAX;
}

int64_t BlockItemAST::const_eval(SymbolTable *stb, bool force) {
  return item->const_eval(stb, force);
}

int64_t DeclAST::const_eval(SymbolTable *stb, bool force) {
  for (const auto &def : *def_l)
    def->const_eval(stb, force | is_const);
  return INT64_MAX;
}

int64_t DefAST::const_eval(SymbolTable *stb, bool force) {
  int64_t val = fold_const(const_init_val, stb, force | is_const);
  assert(stb != nullptr);
  assert(val != INT64_MAX);
  stb->insertConst(*ident, (int32_t)val);
  return INT64_MAX;
}

int64_t InitValAST::const_eval(SymbolTable *stb, bool force) {
  return fold_const(const_exp, stb, force | is_const);
}

int64_t StmtAST::const_eval(SymbolTable *stb, bool force) {
  return fold_const(exp, stb, force), INT64_MAX;
}

int64_t ExpAST::const_eval(SymbolTable *stb, bool force) {
  return fold_const(unary_exp, stb, force | is_const);
}

int64_t PrimaryExpAST::const_eval(SymbolTable *stb, bool force) {
  return fold_const(exp, stb, force);
}

int64_t LValAST::const_eval(SymbolTable *stb, bool force) {
  assert(!force || stb != nullptr);
  try {
    return stb->lookupConst(*ident);
  } catch (const flea_compiler_error &) {
    if (force)
      throw;
    return INT64_MAX;
  }
}

int64_t NumberAST::const_eval([[maybe_unused]] SymbolTable *stb,
                              [[maybe_unused]] bool force) {
  return number;
}

int64_t UnaryExpAST::const_eval(SymbolTable *stb, bool force) {
  int64_t val = fold_const(exp, stb, force);
  if (val == INT64_MAX)
    return INT64_MAX;
  switch (unary_op) {
  case 0:
  case '+':
    return val;
  case '-':
    return -val;
  case '!':
    return !val;
  default:
    assert(false);
    __builtin_unreachable();
  }
}

int64_t BinExpAST::const_eval(SymbolTable *stb, bool force) {
  int64_t lhs_val = fold_const(lhs, stb, force);
  if (lhs_val == INT64_MAX)
    return INT64_MAX;
  if (!rhs)
    return lhs_val;
  assert(op != (char)0);
  int64_t rhs_val = fold_const(rhs, stb, force);
  if (rhs_val == INT64_MAX)
    return INT64_MAX;
  int32_t lhs_val32 = static_cast<int32_t>(lhs_val);
  int32_t rhs_val32 = static_cast<int32_t>(rhs_val);
  switch (op) {
  case '+':
    return safe_add(lhs_val32, rhs_val32);
  case '-':
    return safe_sub(lhs_val32, rhs_val32);
  case '*':
    return safe_mul(lhs_val32, rhs_val32);
  case '/':
    return floor_div(lhs_val32, rhs_val32);
  case '%':
    return floor_mod(lhs_val32, rhs_val32);
  case '<':
    return lhs_val32 < rhs_val32;
  case '>':
    return lhs_val32 > rhs_val32;
  case 'l':
    return lhs_val32 <= rhs_val32;
  case 'g':
    return lhs_val32 >= rhs_val32;
  case 'e':
    return lhs_val32 == rhs_val32;
  case 'n':
    return lhs_val32 != rhs_val32;
  default:
    assert(false);
    __builtin_unreachable();
  }
}