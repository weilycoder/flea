#include "flea_ast.hpp"
#include "flea_sym.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<CompUnitAST> &ast);

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <input>" << endl;
    return 1;
  }
  auto input = argv[1];

  yyin = fopen(input, "r");
  if (!yyin) {
    cerr << "Failed to open file: " << input << endl;
    return 1;
  }

  unique_ptr<CompUnitAST> ast;
  int ret = yyparse(ast);
  if (ret != 0)
    return fclose(yyin), ret;

  cout << *ast << endl;

  // semanticAnalysis
  ast->const_eval(nullptr);
  cout << *ast << endl;

  fclose(yyin);
  return 0;
}