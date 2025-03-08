#ifndef FLEA_SYM_HPP_FLAG
#define FLEA_SYM_HPP_FLAG

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class FuncSign {
public:
  char returnType;
  std::vector<char> argTypes;
  FuncSign(char returnType, const std::vector<char> &argTypes);
  bool operator==(const FuncSign &other) const;
};

using Value = std::variant<int32_t, int64_t, FuncSign>;

class SymbolTable {
protected:
  int64_t offset = 0;
  const SymbolTable *parent;
  std::unordered_map<std::string, Value> table;

public:
  SymbolTable(const SymbolTable *parent = nullptr);
  void clear();
  void insert(const std::string &name, const Value &val);
  void insertConst(const std::string &name, int32_t val);
  void insertVar(const std::string &name);
  void insertVar(const std::string &name, int64_t offset);
  void insertFunc(const std::string &name, const FuncSign &val);
  const Value &lookup(const std::string &name) const;
  FuncSign lookupFunc(const std::string &name) const;
  int32_t lookupConst(const std::string &name) const;
  friend std::ostream &operator<<(std::ostream &os, const SymbolTable &symtab);
};

#endif // FLEA_SYM_HPP_FLAG
