#pragma once
#ifndef FLEA_AST_HPP_FLAG
#define FLEA_AST_HPP_FLAG

#include <iostream>
#include <memory>

class BaseAST {
public:
  virtual ~BaseAST() = default;
  virtual void print(std::ostream &out) const = 0;
};

class CompUnitAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> func_def;
  void print(std::ostream &out) const override;
};

class FuncDefAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void print(std::ostream &out) const override;
};

class FuncTypeAST : public BaseAST {
public:
  std::string ident;
  void print(std::ostream &out) const override;
};

class BlockAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> stmt;
  void print(std::ostream &out) const override;
};

class StmtAST : public BaseAST {
public:
  int32_t number;
  void print(std::ostream &out) const override;
};

#endif // FLEA_AST_HPP_FLAG
