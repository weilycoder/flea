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
  char char_val;
  std::string *str_val;
  BaseAST *ast_val;
}

%token INT RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp Number UnaryExp
%type <char_val> UnaryOp

%%

CompUnit
  : FuncDef {
    using namespace std;
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    using namespace std;
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->ident = "int";
    $$ = ast;
  }
  ;

Block
  : '{' Stmt '}' {
    using namespace std;
    auto ast = new BlockAST();
    ast->stmt = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->exp = std::unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Exp
  : UnaryExp {
    using namespace std;
    auto ast = new ExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->exp = std::unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->exp = std::unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->number = $1;
    $$ = ast;
  }
  ;

UnaryOp
  : '+' { $$ = '+'; }
  | '-' { $$ = '-'; }
  | '!' { $$ = '!'; }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->unary_op = 0;
    ast->exp = std::unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST();
    ast->unary_op = $1;
    ast->exp = std::unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;


%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  std::cerr << "error: " << s << std::endl;
}