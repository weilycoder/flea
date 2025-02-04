%code requires {
  #include <memory>
  #include <string>
}

%{

#include "flea_ast.hpp"
#include <iostream>
#include <memory>
#include <string>

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  int int_val;
  std::string *str_val;
  BaseAST *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp Number UnaryExp

%%

CompUnit
  : FuncDef {
    using namespace std;
    auto comp_unit = make_unique<CompUnitAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    $$ = new FuncDefAST($1, $2, $5);
  }
  ;

FuncType : INT { $$ = new FuncTypeAST("int"); } ;

Block : '{' Stmt '}' { $$ = new BlockAST($2); } ;

Stmt : RETURN Exp ';' { $$ = new StmtAST($2); } ;

Exp : UnaryExp { $$ = new ExpAST($1); } ;

PrimaryExp
  : '(' Exp ')' { $$ = new PrimaryExpAST($2); }
  | Number { $$ = new PrimaryExpAST($1); }
  ;

Number : INT_CONST { $$ = new NumberAST($1); } ;

UnaryExp
  : PrimaryExp { $$ = new UnaryExpAST(0, $1); }
  | '+' UnaryExp { $$ = new UnaryExpAST('+', $2); }
  | '-' UnaryExp { $$ = new UnaryExpAST('-', $2); }
  | '!' UnaryExp { $$ = new UnaryExpAST('!', $2); }
  ;


%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  std::cerr << "error: " << s << std::endl;
}