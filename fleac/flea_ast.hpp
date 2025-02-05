#pragma once
#ifndef FLEA_AST_HPP_FLAG
#define FLEA_AST_HPP_FLAG

#define AST_INDENT 2

#include <iostream>
#include <memory>
#include <vector>

class BaseAST {
public:
  BaseAST() = default;
  virtual ~BaseAST() = default;
  virtual void print(std::ostream &out) const = 0;
  friend std::ostream &operator<<(std::ostream &out, const BaseAST &ast);
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
public:
  CompUnitAST(BaseAST *func_def) : func_def(func_def) {}
  std::unique_ptr<BaseAST> func_def;
  void print(std::ostream &out) const override;
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
  void print(std::ostream &out) const override;
};

// Block ::= "{" {BlockItem} "}";
class BlockAST : public BaseAST {
public:
  BlockAST(std::vector<std::unique_ptr<BaseAST>> *item_l) : item_l(item_l) {}
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> item_l;
  void print(std::ostream &out) const override;
};

// BlockItem ::= Decl | Stmt;
class BlockItemAST : public BaseAST {
public:
  BlockItemAST(BaseAST *item) : item(item) {}
  std::unique_ptr<BaseAST> item;
  void print(std::ostream &out) const override;
};

// Decl ::= ConstDecl;
class DeclAST : public BaseAST {
public:
  DeclAST(BaseAST *decl) : decl(decl) {}
  std::unique_ptr<BaseAST> decl;
  void print(std::ostream &out) const override;
};

// BType ::= "int";
// ConstDecl ::= "const" BType ConstDef {"," ConstDef} ";";
class ConstDeclAST : public BaseAST {
public:
  ConstDeclAST(char type_id, std::vector<std::unique_ptr<BaseAST>> *def_l)
      : type_id(type_id), def_l(def_l) {}
  char type_id;
  std::unique_ptr<std::vector<std::unique_ptr<BaseAST>>> def_l;
  void print(std::ostream &out) const override;
};

// ConstDef ::= IDENT "=" ConstInitVal;
class ConstDefAST : public BaseAST {
public:
  ConstDefAST(std::string *ident, BaseAST *const_init_val)
      : ident(ident), const_init_val(const_init_val) {}
  std::unique_ptr<std::string> ident;
  std::unique_ptr<BaseAST> const_init_val;
  void print(std::ostream &out) const override;
};

// ConstInitVal ::= ConstExp;
class ConstInitValAST : public BaseAST {
public:
  ConstInitValAST(BaseAST *const_exp) : const_exp(const_exp) {}
  std::unique_ptr<BaseAST> const_exp;
  void print(std::ostream &out) const override;
};

// Stmt ::= "return" Exp ";";
class StmtAST : public BaseAST {
public:
  StmtAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override;
};

// Exp ::= EqExp;
class ExpAST : public BaseAST {
public:
  ExpAST(BaseAST *unary_exp) : unary_exp(unary_exp) {}
  std::unique_ptr<BaseAST> unary_exp;
  void print(std::ostream &out) const override;
};

// ConstExp ::= Exp;
class ConstExpAST : public BaseAST {
public:
  ConstExpAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override;
};

// PrimaryExp ::= "(" Exp ")" | LVal | Number;
class PrimaryExpAST : public BaseAST {
public:
  PrimaryExpAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override;
};

// LVal ::= IDENT;
class LValAST : public BaseAST {
public:
  LValAST(std::string *ident) : ident(ident) {}
  std::unique_ptr<std::string> ident;
  void print(std::ostream &out) const override;
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
public:
  NumberAST(int32_t number) : number(number) {}
  int32_t number;
  void print(std::ostream &out) const override;
};

// UnaryOp ::= "+" | "-" | "!";
// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST {
public:
  UnaryExpAST(char unary_op, BaseAST *exp) : unary_op(unary_op), exp(exp) {}
  char unary_op;
  std::unique_ptr<BaseAST> exp;
  void print(std::ostream &out) const override;
};

class BinExpAST : public BaseAST {
protected:
  virtual const char *name() const = 0;

public:
  char op;
  std::unique_ptr<BaseAST> lhs;
  std::unique_ptr<BaseAST> rhs;

  BinExpAST(char op, BaseAST *lhs, BaseAST *rhs) : op(op), lhs(lhs), rhs(rhs) {}
  void print(std::ostream &out) const override;
};

// MulExp ::= UnaryExp | (MulExp ("*" | "/" | "%") UnaryExp);
class MulExpAST : public BinExpAST {
protected:
  const char *name() const override { return "MulExp"; }

public:
  MulExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

// AddExp ::= MulExp | (AddExp ("+" | "-") MulExp);
class AddExpAST : public BinExpAST {
protected:
  const char *name() const override { return "AddExp"; }

public:
  AddExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

// RelExp ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
class RelExpAST : public BinExpAST {
protected:
  const char *name() const override { return "RelExp"; }

public:
  RelExpAST(char id, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(id, lhs, rhs) {}
};

// EqExp ::= RelExp | EqExp ("==" | "!=") RelExp;
class EqExpAST : public BinExpAST {
protected:
  const char *name() const override { return "EqExp"; }

public:
  EqExpAST(char op, BaseAST *lhs, BaseAST *rhs = nullptr)
      : BinExpAST(op, lhs, rhs) {}
};

#endif // FLEA_AST_HPP_FLAG
