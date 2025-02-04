#pragma once
#ifndef FLEA_AST_HPP_FLAG
#define FLEA_AST_HPP_FLAG

#define AST_INDENT 2

#include <iostream>
#include <memory>

std::ostream &print_indent(std::ostream &out, size_t indent);

class BaseAST {
public:
  virtual ~BaseAST() = default;
  virtual std::ostream &print(std::ostream &out, size_t indent = 0) const = 0;
  friend std::ostream &operator<<(std::ostream &out, const BaseAST &ast);
};

class CompUnitAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> func_def;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

class FuncDefAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

class FuncTypeAST : public BaseAST {
public:
  std::string ident;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

class BlockAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> stmt;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

class StmtAST : public BaseAST {
public:
  int32_t number;
  std::ostream &print(std::ostream &out, size_t indent = 0) const override;
};

#endif // FLEA_AST_HPP_FLAG
