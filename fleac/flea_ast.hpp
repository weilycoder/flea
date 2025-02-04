#pragma once
#ifndef FLEA_AST_HPP_FLAG
#define FLEA_AST_HPP_FLAG

#define AST_INDENT 2

#include <iostream>
#include <memory>

class BaseAST {
public:
  BaseAST() = default;
  virtual ~BaseAST() = default;
  virtual std::ostream &print(std::ostream &out, size_t indent = 0) const = 0;
  friend std::ostream &operator<<(std::ostream &out, const BaseAST &ast);
};

// CompUnit ::= FuncDef;
class CompUnitAST : public BaseAST {
public:
  CompUnitAST(BaseAST *func_def) : func_def(func_def) {}
  std::unique_ptr<BaseAST> func_def;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// FuncDef ::= FuncType IDENT "(" ")" Block;
class FuncDefAST : public BaseAST {
public:
  FuncDefAST(BaseAST *func_type, const std::string *ident, BaseAST *block)
      : func_type(func_type), ident(*ident), block(block) {}
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// FuncType ::= "int";
class FuncTypeAST : public BaseAST {
public:
  FuncTypeAST(const std::string &ident) : ident(ident) {}
  std::string ident;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// Block ::= "{" Stmt "}";
class BlockAST : public BaseAST {
public:
  BlockAST(BaseAST *stmt) : stmt(stmt) {}
  std::unique_ptr<BaseAST> stmt;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// Stmt ::= "return" Exp ";";
class StmtAST : public BaseAST {
public:
  StmtAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// Exp ::= UnaryExp;
class ExpAST : public BaseAST {
public:
  ExpAST(BaseAST *unary_exp) : unary_exp(unary_exp) {}
  std::unique_ptr<BaseAST> unary_exp;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// PrimaryExp ::= "(" Exp ")" | Number;
class PrimaryExpAST : public BaseAST {
public:
  PrimaryExpAST(BaseAST *exp) : exp(exp) {}
  std::unique_ptr<BaseAST> exp;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST {
public:
  NumberAST(int32_t number) : number(number) {}
  int32_t number;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

// UnaryOp ::= "+" | "-" | "!";
// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST {
public:
  UnaryExpAST(char unary_op, BaseAST *exp) : unary_op(unary_op), exp(exp) {}
  char unary_op;
  std::unique_ptr<BaseAST> exp;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

#endif // FLEA_AST_HPP_FLAG
