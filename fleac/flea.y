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
  std::vector<std::unique_ptr<BaseAST>> *list_val;
}

%token INT CONST RETURN LT GT LE GE EQ NE
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef
%type <char_val> FuncType BType
%type <list_val> BlockItemList ConstDefList
%type <ast_val> Block Stmt
%type <ast_val> BlockItem
%type <ast_val> Decl ConstDecl ConstDef ConstInitVal
%type <ast_val> Exp ConstExp Number LVal
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

BType : INT { $$ = (char)1; } ;

FuncType : INT { $$ = (char)1; } ;

Block : '{' BlockItemList '}' { $$ = new BlockAST($2); } ;

BlockItem
  : Decl { $$ = new BlockItemAST($1); }
  | Stmt { $$ = new BlockItemAST($1); } ;

BlockItemList
  : { $$ = new std::vector<std::unique_ptr<BaseAST>>; }
  | BlockItemList BlockItem {
    $1->emplace_back($2);
    $$ = $1;
  }
  ;

Decl : ConstDecl { $$ = new DeclAST($1); } ;

ConstDecl : CONST BType ConstDefList ';' { $$ = new ConstDeclAST($2, $3); } ;

ConstDef : IDENT '=' ConstInitVal { $$ = new ConstDefAST($1, $3); } ;

ConstDefList
  : ConstDef {
    auto def_l = new std::vector<std::unique_ptr<BaseAST>>;
    def_l->emplace_back($1);
    $$ = def_l;
  }
  | ConstDefList ',' ConstDef {
    $1->emplace_back($3);
    $$ = $1;
  }
  ;

ConstInitVal : ConstExp { $$ = new ConstInitValAST($1); } ;

Stmt : RETURN Exp ';' { $$ = new StmtAST($2); } ;

Exp : EqExp { $$ = new ExpAST($1); } ;

ConstExp : Exp { $$ = new ConstExpAST($1); } ;

LVal : IDENT { $$ = new LValAST($1); } ;

Number : INT_CONST { $$ = new NumberAST($1); } ;

PrimaryExp
  : '(' Exp ')' { $$ = new PrimaryExpAST($2); }
  | LVal { $$ = new PrimaryExpAST($1); }
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