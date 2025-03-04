#ifndef FLEA_SYM_HPP_FLAG
#define FLEA_SYM_HPP_FLAG

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>

using Value = std::variant<int32_t, uint32_t, char>;

class SymbolTable {
protected:
  uint32_t offset = 0;
  const SymbolTable *parent;
  std::unordered_map<std::string, Value> table;

public:
  SymbolTable(const SymbolTable *parent = nullptr);
  void clear();
  void insert(const std::string &name, const Value &val);
  void insertConst(const std::string &name, int32_t val);
  void insertVar(const std::string &name);
  void insertFunc(const std::string &name, char val);
  const Value &lookup(const std::string &name) const;
  char lookupFunc(const std::string &name) const;
  int32_t lookupConst(const std::string &name) const;
  friend std::ostream &operator<<(std::ostream &os, const SymbolTable &symtab);
};

#endif // FLEA_SYM_HPP_FLAG
