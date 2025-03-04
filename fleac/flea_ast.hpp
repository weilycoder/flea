#pragma once
#ifndef FLEA_AST_HPP_FLAG
#define FLEA_AST_HPP_FLAG

#define AST_INDENT 2

#include "flea_sym.hpp"
#include <iostream>
#include <memory>
#include <vector>

class BaseAST {
public:
  virtual ~BaseAST() = default;
  virtual void print(std::ostream &out) const = 0;
  virtual int64_t const_eval(SymbolTable *stb, uint32_t context = 0) = 0;
  friend std::ostream &operator<<(std::ostream &out, const BaseAST &ast);
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
public:
  CompUnitAST(BaseAST *func_def) : func_def(func_def) {}
  std::unique_ptr<BaseAST> func_def;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// FuncType ::= "int";
// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
public:
  FuncDefAST(char type_id, std::string *ident, BaseAST *block)
      : func_type_id(type_id), ident(ident), block(block) {}
  char func_type_id;
  std::unique_ptr<std::string> ident;
  std::unique_ptr<BaseAST> block;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// Block ::= "{" {BlockItem} "}";
class BlockAST : public BaseAST {
public:
  BlockAST(std::vector<std::unique_ptr<BaseAST>> *item_l) : item_l(item_l) {}
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> item_l;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// BlockItem ::= Decl | Stmt;
// class BlockItemAST : public BaseAST {
// public:
//   BlockItemAST(BaseAST *item) : item(item) {}
//   std::unique_ptr<BaseAST> item;
//   void print(std::ostream &out) const override final;
//   int64_t const_eval(SymbolTable *stb, bool force = false) override final;
// };

// BType ::= "int";
// Decl ::= ConstDecl | VarDecl;
// VarDecl ::= BType VarDef {"," VarDef} ";";
// ConstDecl ::= "const" BType ConstDef {"," ConstDef} ";";
class DeclAST : public BaseAST {
public:
  DeclAST(char type_id, std::vector<std::unique_ptr<BaseAST>> *def_l,
          bool is_const = false)
      : is_const(is_const), type_id(type_id), def_l(def_l) {}
  bool is_const;
  char type_id;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> def_l;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// VarDef ::= IDENT "=" InitVal;
// ConstDef ::= IDENT "=" ConstInitVal;
class DefAST : public BaseAST {
public:
  DefAST(std::string *ident, BaseAST *init_val = nullptr, bool is_const = false)
      : is_const(is_const), ident(ident), init_val(init_val) {}
  bool is_const;
  std::unique_ptr<std::string> ident;
  std::unique_ptr<BaseAST> init_val;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// InitVal ::= Exp;
// ConstInitVal ::= ConstExp;
class InitValAST : public BaseAST {
public:
  InitValAST(BaseAST *exp, bool is_const = false)
      : is_const(is_const), exp(exp) {}
  bool is_const;
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// class StmtAST : public BaseAST {
// public:
//   StmtAST(BaseAST *stmt) : stmt(stmt) {}
//   std::unique_ptr<BaseAST> stmt;
//   void print(std::ostream &out) const override final;
//   int64_t const_eval(SymbolTable *stb, bool force = false) override final;
// };

// ExpStmt ::= Exp ";";
class ExpStmtAST : public BaseAST {
public:
  ExpStmtAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// RetStmt ::= "return" Exp ";";
class RetStmtAST : public BaseAST {
public:
  RetStmtAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// AssignStmt ::= LVal "=" Exp ";";
class AssignStmtAST : public BaseAST {
public:
  AssignStmtAST(BaseAST *lval, BaseAST *exp) : lval(lval), exp(exp) {}
  std::unique_ptr<BaseAST> lval;
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

class IfStmtAST : public BaseAST {
public:
  IfStmtAST(BaseAST *cond, BaseAST *then_stmt = nullptr,
            BaseAST *else_stmt = nullptr)
      : cond(cond), then_stmt(then_stmt), else_stmt(else_stmt) {}
  std::unique_ptr<BaseAST> cond;
  std::unique_ptr<BaseAST> then_stmt;
  std::unique_ptr<BaseAST> else_stmt;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

class WhileStmtAST : public BaseAST {
public:
  WhileStmtAST(BaseAST *cond, BaseAST *stmt = nullptr)
      : cond(cond), stmt(stmt) {}
  std::unique_ptr<BaseAST> cond;
  std::unique_ptr<BaseAST> stmt;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// Exp ::= EqExp;
// ConstExp ::= Exp;
class ExpAST : public BaseAST {
public:
  ExpAST(BaseAST *exp, bool is_const = false) : is_const(is_const), exp(exp) {}
  bool is_const;
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
class PrimaryExpAST : public BaseAST {
public:
  PrimaryExpAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// LVal ::= IDENT;
class LValAST : public BaseAST {
public:
  LValAST(std::string *ident) : ident(ident) {}
  std::unique_ptr<std::string> ident;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
  bool is_const(SymbolTable *stb) const;
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
public:
  NumberAST(int32_t number) : number(number) {}
  int32_t number;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// UnaryOp ::= "+" | "-" | "!";
// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST {
public:
  UnaryExpAST(char unary_op, BaseAST *exp) : unary_op(unary_op), exp(exp) {}
  char unary_op;
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

class BinExpAST : public BaseAST {
protected:
  virtual const char *name() const = 0;

public:
  char op;
  std::unique_ptr<BaseAST> lhs;
  std::unique_ptr<BaseAST> rhs;

  BinExpAST(char op, BaseAST *lhs, BaseAST *rhs) : op(op), lhs(lhs), rhs(rhs) {}
  void print(std::ostream &out) const override final;
  int64_t const_eval(SymbolTable *stb, uint32_t context = 0) override final;
};

// MulExp ::= UnaryExp | (MulExp ("*" | "/" | "%") UnaryExp);
class MulExpAST : public BinExpAST {
protected:
  const char *name() const override final { return "MulExp"; }

public:
  MulExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

// AddExp ::= MulExp | (AddExp ("+" | "-") MulExp);
class AddExpAST : public BinExpAST {
protected:
  const char *name() const override final { return "AddExp"; }

public:
  AddExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

// RelExp ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
class RelExpAST : public BinExpAST {
protected:
  const char *name() const override final { return "RelExp"; }

public:
  RelExpAST(char id, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(id, lhs, rhs) {}
};

// EqExp ::= RelExp | EqExp ("==" | "!=") RelExp;
class EqExpAST : public BinExpAST {
protected:
  const char *name() const override final { return "EqExp"; }

public:
  EqExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

#endif // FLEA_AST_HPP_FLAG
