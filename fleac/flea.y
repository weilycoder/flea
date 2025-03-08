%code requires {
  #include <cassert>
  #include <memory>
  #include <string>
}

%{

#include "flea_ast.hpp"
#include <iostream>
#include <memory>
#include <string>

extern int yylineno;

int yylex();
void yyerror(const char *msg, int yylineno);
void yyerror(std::unique_ptr<BaseAST> &ast, const char *msg);

%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  char char_val;
  int32_t int_val;
  std::string *str_val;
  BaseAST *ast_val;
  std::vector<std::unique_ptr<BaseAST>> *list_val;
}

%token INT VOID CONST RETURN IF ELSE WHILE BREAK CONTINUE LT GT LE GE EQ NE
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncFParam
%type <char_val> FuncType BType
%type <list_val> BlockItemList VarDefList ConstDefList FuncFParamsList FuncRParamsList
%type <ast_val> Block Stmt
%type <ast_val> ExpStmt RetStmt AssignStmt BreakStmt ContinueStmt
%type <ast_val> OpenStmt ClosedStmt SimpleStmt
%type <ast_val> OpenIf ClosedIf
%type <ast_val> OpenWhile ClosedWhile
%type <ast_val> BlockItem
%type <ast_val> Decl VarDecl VarDef InitVal ConstDecl ConstDef ConstInitVal
%type <ast_val> Exp ConstExp Number LVal
%type <ast_val> PrimaryExp UnaryExp MulExp AddExp RelExp EqExp CallExp

%destructor { delete $$; } <str_val>
%destructor { delete $$; } <ast_val>
%destructor { delete $$; } <list_val>

%%

CompUnit
  : { ast = std::make_unique<CompUnitAST>(); }
  | CompUnit FuncDef {
    dynamic_cast<CompUnitAST *>(ast.get())->insertFunc($2);
  }
  ;

FuncDef
  : FuncType IDENT '(' FuncFParamsList ')' Block {
    $$ = new FuncDefAST($1, $2, $6, $4);
  }
  ;

FuncFParamsList
  : { $$ = new std::vector<std::unique_ptr<BaseAST>>; }
  | FuncFParam {
    auto fp_l = new std::vector<std::unique_ptr<BaseAST>>;
    fp_l->emplace_back($1);
    $$ = fp_l;
  }
  | FuncFParamsList ',' FuncFParam {
    $1->emplace_back($3);
    $$ = $1;
  }
  ;

FuncFParam : BType IDENT { $$ = new FuncFParamAST($1, $2); } ;

BType : INT { $$ = (char)1; } ;

FuncType
  : INT { $$ = (char)1; }
  | VOID { $$ = (char)0; }
  ;

Block : '{' BlockItemList '}' { $$ = new BlockAST($2); } ;

BlockItem
  : Decl { $$ = $1; }
  | Stmt { $$ = $1; } ;

BlockItemList
  : { $$ = new std::vector<std::unique_ptr<BaseAST>>; }
  | BlockItemList BlockItem {
    if ($2) $1->emplace_back($2);
    $$ = $1;
  }
  ;

Decl
  : VarDecl { $$ = $1; }
  | ConstDecl { $$ = $1; }
  ;

VarDecl : BType VarDefList ';' { $$ = new DeclAST($1, $2); }

VarDef
  : IDENT {
    $$ = new DefAST($1);
  }
  | IDENT '=' InitVal {
    $$ = new DefAST($1, $3);
  }
  ;

VarDefList
  : VarDef {
    auto def_l = new std::vector<std::unique_ptr<BaseAST>>;
    def_l->emplace_back($1);
    $$ = def_l;
  }
  | VarDefList ',' VarDef {
    $1->emplace_back($3);
    $$ = $1;
  }
  ;

ConstDecl : CONST BType ConstDefList ';' { $$ = new DeclAST($2, $3, true); } ;

ConstDef : IDENT '=' ConstInitVal { $$ = new DefAST($1, $3, true); } ;

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

InitVal : Exp { $$ = new InitValAST($1); } ;

ConstInitVal : ConstExp { $$ = new InitValAST($1, true); } ;

Stmt
  : OpenStmt { $$ = $1; }
  | ClosedStmt { $$ = $1; }
  ;

OpenStmt
  : OpenIf { $$ = $1; }
  | OpenWhile { $$ = $1; }
  ;

ClosedStmt
  : SimpleStmt { $$ = $1; }
  | ClosedIf { $$ = $1; }
  | ClosedWhile { $$ = $1; }
  ;

OpenIf
  : IF '(' Exp ')' Stmt {
    $$ = new IfStmtAST($3, $5);
  }
  | IF '(' Exp ')' ClosedStmt ELSE OpenStmt {
    $$ = new IfStmtAST($3, $5, $7);
  }
  ;

ClosedIf
  : IF '(' Exp ')' ClosedStmt ELSE ClosedStmt {
    $$ = new IfStmtAST($3, $5, $7);
  }
  ;

OpenWhile : WHILE '(' Exp ')' OpenStmt { $$ = new WhileStmtAST($3, $5); } ;

ClosedWhile : WHILE '(' Exp ')' ClosedStmt { $$ = new WhileStmtAST($3, $5); } ;

SimpleStmt
  : ';' { $$ = nullptr; }
  | Block { $$ = $1; }
  | ExpStmt { $$ = $1; }
  | RetStmt { $$ = $1; }
  | BreakStmt { $$ = $1; }
  | ContinueStmt { $$ = $1; }
  | AssignStmt { $$ = $1; }
  ;

BreakStmt : BREAK ';' { $$ = new BreakStmtAST(); } ;

ContinueStmt : CONTINUE ';' { $$ = new ContinueStmtAST(); } ;

RetStmt : RETURN Exp ';' { $$ = new RetStmtAST($2); } ;

ExpStmt : Exp ';' { $$ = new ExpStmtAST($1); } ;

AssignStmt : LVal '=' Exp ';' { $$ = new AssignStmtAST($1, $3); } ;

Exp : EqExp { $$ = new ExpAST($1); } ;

ConstExp : Exp { $$ = new ExpAST($1, true); } ;

LVal : IDENT { $$ = new LValAST($1); } ;

Number : INT_CONST { $$ = new NumberAST($1); } ;

CallExp : IDENT '(' FuncRParamsList ')' { $$ = new CallExpAST($1, $3); } ;

FuncRParamsList
  : { $$ = new std::vector<std::unique_ptr<BaseAST>>; }
  | Exp {
    auto fp_l = new std::vector<std::unique_ptr<BaseAST>>;
    fp_l->emplace_back($1);
    $$ = fp_l;
  }
  | FuncRParamsList ',' Exp {
    $1->emplace_back($3);
    $$ = $1;
  }
  ;

PrimaryExp
  : '(' Exp ')' { $$ = new PrimaryExpAST($2); }
  | LVal { $$ = $1; }
  | Number { $$ = $1; }
  | CallExp { $$ = $1; }
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

void yyerror(const char *msg, int lineno) {
  std::cerr << "Error at line " << lineno << ": " << msg << std::endl;
}

void yyerror([[maybe_unused]] std::unique_ptr<BaseAST> &ast, const char *msg) {
  yyerror(msg, yylineno);
}