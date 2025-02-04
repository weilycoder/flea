#include "flea_ast.hpp"

void CompUnitAST::print(std::ostream &out) const {
  out << "CompUnit { ";
  func_def->print(out);
  out << " }";
}

void FuncDefAST::print(std::ostream &out) const {
  out << "FuncDef { ";
  func_type->print(out);
  out << " " << ident << " ";
  block->print(out);
  out << " }";
}

void FuncTypeAST::print(std::ostream &out) const {
  out << "FuncType { " << ident << " }";
}

void BlockAST::print(std::ostream &out) const {
  out << "Block { ";
  stmt->print(out);
  out << " }";
}

void StmtAST::print(std::ostream &out) const {
  out << "Stmt { ";
  out << number;
  out << " }";
}