cd fleac
flex -o flea.lex.cpp flea.l
bison -d -o flea.tab.cpp flea.y
clang++ -o fleac flea.tab.cpp flea.lex.cpp flea_ast.cpp flea_expr.cpp flea_sym.cpp main.cpp -O3 -std=c++20 -DNDEBUG
