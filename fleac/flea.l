%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string>

#include "flea_ast.hpp"
#include "flea.tab.hpp"

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    { /* ignore */ }
{LineComment}   { /* ignore */ }

"int"           { return INT; }
"const"         { return CONST; }
"return"        { return RETURN; }

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