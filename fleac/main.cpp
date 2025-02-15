#include "flea_ast.hpp"
#include "flea_sym.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  assert(argc == 2);
  auto input = argv[1];

  yyin = fopen(input, "r");
  assert(yyin);

  unique_ptr<BaseAST> ast;
  int ret = yyparse(ast);
  if (ret != 0)
    return ret;

  cout << *ast << endl;

  // semanticAnalysis
  SymbolTable *symtab = new SymbolTable();
  ast->const_eval(symtab);
  cout << *symtab;
  cout << *ast << endl;
  return delete symtab, 0;
}