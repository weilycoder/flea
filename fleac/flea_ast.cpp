#include "flea_ast.hpp"
#include "flea_expr.hpp"
#include <cassert>

// print functions

void CompUnitAST::insertFunc(BaseAST *func_def) {
  auto func_def_ast = dynamic_cast<FuncDefAST *>(func_def);
  assert(func_def_ast);
  std::vector<char> arg_types;
  for (const auto &fparam : *func_def_ast->fparam_l) {
    auto fparam_ast = dynamic_cast<FuncFParamAST *>(fparam.get());
    assert(fparam_ast);
    arg_types.push_back(fparam_ast->type_id);
  }
  FuncSign func_sign(func_def_ast->func_type_id, arg_types);
  stb.insertFunc(*func_def_ast->ident, func_sign);
  func_def_l.push_back(std::unique_ptr<BaseAST>(func_def));
}

void CompUnitAST::print(std::ostream &out) const {
  for (const auto &func_def : func_def_l)
    func_def->print(out), out << '\n';
}

void FuncDefAST::print(std::ostream &out) const {
  out << id2tp(func_type_id) << ' ' << *ident;
  out << "(";
  for (size_t i = 0; i < fparam_l->size(); ++i) {
    if (i != 0)
      out << ", ";
    (*fparam_l)[i]->print(out);
  }
  out << ") ";
  block->print(out);
}

void FuncFParamAST::print(std::ostream &out) const {
  out << id2tp(type_id) << ' ' << *ident;
}

void BlockAST::print(std::ostream &out) const {
  out << "{ ";
  for (const auto &item : *item_l)
    item->print(out), out << ' ';
  out << "}";
}

// void BlockItemAST::print(std::ostream &out) const { item->print(out); }

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
  out << *ident;
  if (init_val)
    out << " = ", init_val->print(out);
}

void InitValAST::print(std::ostream &out) const { exp->print(out); }

// void StmtAST::print(std::ostream &out) const { stmt->print(out); }

void ExpStmtAST::print(std::ostream &out) const { exp->print(out), out << ";"; }

void RetStmtAST::print(std::ostream &out) const {
  out << "return ";
  exp->print(out);
  out << ";";
}

void BreakStmtAST::print(std::ostream &out) const { out << "break;"; }

void ContinueStmtAST::print(std::ostream &out) const { out << "continue;"; }

void AssignStmtAST::print(std::ostream &out) const {
  lval->print(out);
  out << " = ";
  exp->print(out);
  out << ";";
}

void IfStmtAST::print(std::ostream &out) const {
  out << "if (";
  cond->print(out);
  out << ") ";
  if (then_stmt)
    then_stmt->print(out);
  else
    out << ";";
  if (else_stmt) {
    out << " else ";
    else_stmt->print(out);
  }
}

void WhileStmtAST::print(std::ostream &out) const {
  out << "while (";
  cond->print(out);
  out << ") ";
  if (stmt)
    stmt->print(out);
  else
    out << ";";
}

void ExpAST::print(std::ostream &out) const { exp->print(out); }

void PrimaryExpAST::print(std::ostream &out) const {
  out << "(";
  exp->print(out);
  out << ")";
}

void LValAST::print(std::ostream &out) const { out << *ident; }

void NumberAST::print(std::ostream &out) const { out << number; }

void CallExpAST::print(std::ostream &out) const {
  out << *ident << "(";
  for (size_t i = 0; i < fparam_l->size(); ++i) {
    if (i != 0)
      out << ", ";
    (*fparam_l)[i]->print(out);
  }
  out << ")";
}

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

inline bool check_force(uint64_t context) { return context & 1; }
inline uint64_t &set_force(uint64_t &context) { return context |= 1; }
inline bool check_loop(uint64_t context) { return context & 2; }
inline uint64_t &set_loop(uint64_t &context) { return context |= 2; }
inline uint64_t check_con_type(uint64_t context) { return (context >> 2) & 3; }
inline uint64_t check_ret_type(uint64_t context) { return (context >> 4) & 3; }
inline uint64_t set_con_type(uint64_t &context, uint64_t type) {
  assert(type < 4);
  return context = (context & ~static_cast<uint64_t>(3 << 2)) | (type << 2);
}
inline uint64_t set_ret_type(uint64_t &context, uint64_t type) {
  assert(type < 4);
  return context = (context & ~static_cast<uint64_t>(3 << 4)) | (type << 4);
}

constexpr int64_t VOID_VAR = INT64_MAX;
constexpr int64_t INT_VAR = static_cast<int64_t>(1) << 32;
constexpr int64_t ARR_VAR = static_cast<int64_t>(2) << 32;

void check_and_raise(int64_t val) {
  if (val == VOID_VAR)
    throw flea_compiler_error("void value in const expression");
  if (val == ARR_VAR)
    throw flea_compiler_error("array value in const expression");
}

inline bool check_const(int64_t val) {
  return val <= INT32_MAX && val >= INT32_MIN;
}

inline uint64_t get_type(int64_t val) {
  if (val == VOID_VAR)
    return VOID;
  if (val == INT_VAR)
    return INT;
  if (val == ARR_VAR)
    return ARR;
  return INT;
}

int64_t fold_const(std::unique_ptr<BaseAST> &ast, SymbolTable *stb,
                   uint64_t context) {
  int64_t val = ast->const_eval(stb, context);
  if (check_force(context))
    assert(check_const(val));
  if (!check_const(val))
    return val;
  ast.reset(new NumberAST((int32_t)val));
  return val;
}

int64_t CompUnitAST::const_eval(SymbolTable *stb, uint64_t context) {
  stb = &this->stb;
  for (const auto &func_def : func_def_l) {
    SymbolTable new_stb(stb);
    func_def->const_eval(&new_stb, context);
  }
  return VOID_VAR;
}

int64_t FuncDefAST::const_eval(SymbolTable *stb, uint64_t context) {
  uint64_t offset = 0;
  set_ret_type(context, static_cast<uint64_t>(func_type_id));
  for (const auto &fparam : *fparam_l)
    fparam->const_eval(stb, context | (offset++ << 32));
  return block->const_eval(stb, context);
}

int64_t FuncFParamAST::const_eval(SymbolTable *stb, uint64_t context) {
  int64_t offset = static_cast<int64_t>(context >> 32) + 1;
  stb->insertVar(*ident, -offset);
  return VOID_VAR;
}

int64_t BlockAST::const_eval(SymbolTable *stb, uint64_t context) {
  SymbolTable new_stb(stb);
  for (const auto &item : *item_l)
    item->const_eval(&new_stb, context);
  return VOID_VAR;
}

// int64_t BlockItemAST::const_eval(SymbolTable *stb, bool force) {
//   return item->const_eval(stb, force);
// }

int64_t DeclAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (is_const)
    set_force(context);
  for (const auto &def : *def_l)
    def->const_eval(stb, context);
  return VOID_VAR;
}

int64_t DefAST::const_eval(SymbolTable *stb, uint64_t context) {
  set_con_type(context, static_cast<uint64_t>(INT));
  if (!stb)
    return VOID_VAR;
  if (is_const)
    set_force(context);
  int64_t val = init_val ? fold_const(init_val, stb, context) : VOID_VAR;
  if (is_const)
    stb->insertConst(*ident, static_cast<int32_t>(val));
  else
    stb->insertVar(*ident);
  return VOID_VAR;
}

int64_t InitValAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (is_const)
    set_force(context);
  int64_t val = fold_const(exp, stb, context);
  if (get_type(val) != check_con_type(context))
    throw flea_compiler_error("type mismatch in init expression");
  return val;
}

// int64_t StmtAST::const_eval(SymbolTable *stb, bool force) {
//   return stmt->const_eval(stb, force);
// }

int64_t ExpStmtAST::const_eval(SymbolTable *stb, uint64_t context) {
  return exp->const_eval(stb, context), VOID_VAR;
}

int64_t AssignStmtAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (!stb)
    return VOID_VAR;
  auto lval = dynamic_cast<LValAST *>(this->lval.get());
  assert(lval);
  if (lval->is_const(stb))
    throw flea_compiler_error("cannot assign to const");
  fold_const(exp, stb, context);
  return VOID_VAR;
}

int64_t IfStmtAST::const_eval(SymbolTable *stb, uint64_t context) {
  fold_const(cond, stb, context);
  if (then_stmt)
    then_stmt->const_eval(stb, context);
  if (else_stmt)
    else_stmt->const_eval(stb, context);
  return VOID_VAR;
}

int64_t WhileStmtAST::const_eval(SymbolTable *stb, uint64_t context) {
  set_loop(context);
  fold_const(cond, stb, context);
  if (stmt)
    stmt->const_eval(stb, context);
  return VOID_VAR;
}

int64_t RetStmtAST::const_eval(SymbolTable *stb, uint64_t context) {
  int64_t val = fold_const(exp, stb, context);
  if (get_type(val) != check_ret_type(context))
    throw flea_compiler_error("type mismatch in return expression");
  return VOID_VAR;
}

int64_t BreakStmtAST::const_eval([[maybe_unused]] SymbolTable *stb,
                                 uint64_t context) {
  if (!check_loop(context))
    throw flea_compiler_error("break statement not within loop");
  return VOID_VAR;
}

int64_t ContinueStmtAST::const_eval([[maybe_unused]] SymbolTable *stb,
                                    uint64_t context) {
  if (!check_loop(context))
    throw flea_compiler_error("continue statement not within loop");
  return VOID_VAR;
}

int64_t ExpAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (is_const)
    set_force(context);
  return fold_const(exp, stb, context);
}

int64_t PrimaryExpAST::const_eval(SymbolTable *stb, uint64_t context) {
  return fold_const(exp, stb, context);
}

int64_t LValAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (!stb)
    return INT_VAR;
  try {
    return stb->lookupConst(*ident);
  } catch (const flea_compiler_error &) {
    if (check_force(context))
      throw;
    return stb->lookup(*ident), INT_VAR;
  }
}

int64_t NumberAST::const_eval([[maybe_unused]] SymbolTable *stb,
                              [[maybe_unused]] uint64_t context) {
  return number;
}

int64_t CallExpAST::const_eval(SymbolTable *stb, uint64_t context) {
  if (!stb)
    return INT_VAR;
  if (check_force(context))
    throw flea_compiler_error("function call in const expression");
  auto func_sign = stb->lookupFunc(*ident);
  if (fparam_l->size() != func_sign.argTypes.size())
    throw flea_compiler_error("function call with wrong number of arguments");
  for (size_t i = 0; i < fparam_l->size(); ++i) {
    int64_t val = fold_const((*fparam_l)[i], stb, context);
    if (get_type(val) != static_cast<uint64_t>(func_sign.argTypes[i]))
      throw flea_compiler_error("function call with wrong argument type");
  }
  switch (func_sign.returnType) {
  case static_cast<char>(VOID):
    return VOID_VAR;
  case static_cast<char>(INT):
    return INT_VAR;
  default:
    return ARR_VAR;
  }
}

int64_t UnaryExpAST::const_eval(SymbolTable *stb, uint64_t context) {
  int64_t val = fold_const(exp, stb, context);
  if (unary_op != 0)
    check_and_raise(val);
  if (!check_const(val))
    return val;
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

int64_t BinExpAST::const_eval(SymbolTable *stb, uint64_t context) {
  int64_t lhs_val = fold_const(lhs, stb, context);
  if (!rhs)
    return lhs_val;
  assert(op != (char)0);
  int64_t rhs_val = fold_const(rhs, stb, context);
  check_and_raise(lhs_val), check_and_raise(rhs_val);
  if (lhs_val == INT_VAR || rhs_val == INT_VAR)
    return INT_VAR;
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

bool LValAST::is_const(SymbolTable *stb) const {
  try {
    stb->lookupConst(*ident);
    return true;
  } catch (const flea_compiler_error &) {
    return false;
  }
}
