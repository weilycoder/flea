flex -o flea.lex.cpp flea.l
bison -d -o flea.tab.cpp flea.y
clang++ -o fleac flea.tab.cpp flea.lex.cpp flea_ast.cpp main.cpp -O3 -std=c++20
