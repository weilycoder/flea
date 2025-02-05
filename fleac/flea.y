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

%token INT RETURN LT GT LE GE EQ NE
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef
%type <char_val> FuncType
%type <ast_val> Block Stmt
%type <ast_val> Exp Number
%type <ast_val> PrimaryExp UnaryExp MulExp AddExp RelExp EqExp

%%

CompUnit
  : FuncDef {
    auto comp_unit = std::make_unique<CompUnitAST>($1);
    ast = std::move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    $$ = new FuncDefAST($1, $2, $5);
  }
  ;

FuncType : INT { $$ = (char)1; } ;

Block : '{' Stmt '}' { $$ = new BlockAST($2); } ;

Stmt : RETURN Exp ';' { $$ = new StmtAST($2); } ;

Exp : EqExp { $$ = new ExpAST($1); } ;

Number : INT_CONST { $$ = new NumberAST($1); } ;

PrimaryExp
  : '(' Exp ')' { $$ = new PrimaryExpAST($2); }
  | Number { $$ = new PrimaryExpAST($1); }
  ;

UnaryExp
  : PrimaryExp { $$ = new UnaryExpAST(0, $1); }
  | '+' UnaryExp { $$ = new UnaryExpAST('+', $2); }
  | '-' UnaryExp { $$ = new UnaryExpAST('-', $2); }
  | '!' UnaryExp { $$ = new UnaryExpAST('!', $2); }
  ;

MulExp
  : UnaryExp { $$ = new MulExpAST(0, $1); }
  | MulExp '*' UnaryExp { $$ = new MulExpAST('*', $1, $3); }
  | MulExp '/' UnaryExp { $$ = new MulExpAST('/', $1, $3); }
  | MulExp '%' UnaryExp { $$ = new MulExpAST('%', $1, $3); }
  ;

AddExp
  : MulExp { $$ = new AddExpAST(0, $1); }
  | AddExp '+' MulExp { $$ = new AddExpAST('+', $1, $3); }
  | AddExp '-' MulExp { $$ = new AddExpAST('-', $1, $3); }
  ;

RelExp
  : AddExp { $$ = new RelExpAST(0, $1); }
  | RelExp LT AddExp { $$ = new AddExpAST('<', $1, $3); }
  | RelExp GT AddExp { $$ = new AddExpAST('>', $1, $3); }
  | RelExp LE AddExp { $$ = new AddExpAST('l', $1, $3); }
  | RelExp GE AddExp { $$ = new AddExpAST('g', $1, $3); }
  ;

EqExp
  : RelExp { $$ = new EqExpAST(0, $1); }
  | EqExp EQ RelExp { $$ = new EqExpAST('e', $1, $3); }
  | EqExp NE RelExp { $$ = new EqExpAST('n', $1, $3); }
  ;

%%

void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
  if (ast)
    std::cerr << *ast << std::endl;
  std::cerr << "error: " << s << std::endl;
}