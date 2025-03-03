%option noyywrap
%option nounput
%option noinput
%option yylineno

%{

#include <cstdlib>
#include <string>

#include "flea_ast.hpp"
#include "flea.tab.hpp"

%}

WhiteSpace    [ \t\n\r]*
LineComment   "//".*

Identifier    [a-zA-Z_][a-zA-Z0-9_]*

Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

"\n"            { ++yylineno; }

{WhiteSpace}    { /* ignore */ }
{LineComment}   { /* ignore */ }

"int"           { return INT; }
"const"         { return CONST; }
"return"        { return RETURN; }
"if"            { return IF; }
"else"          { return ELSE; }

{Identifier}    { yylval.str_val = new std::string(yytext); return IDENT; }

{Decimal}       { yylval.int_val = (int32_t)strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = (int32_t)strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = (int32_t)strtol(yytext, nullptr, 0); return INT_CONST; }

"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"!="            { return NE; }

.               { return yytext[0]; }

%%