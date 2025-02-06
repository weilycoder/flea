#ifndef FLEA_SYM_HPP_FLAG
#define FLEA_SYM_HPP_FLAG

#include "flea_err.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

struct Value {
  int32_t val;
  enum class Kind { CONST, VAR, FUNC } kind;
};

class SymbolTable {
protected:
  const SymbolTable *parent;
  std::unordered_map<std::string, Value> table;

public:
  SymbolTable(const SymbolTable *parent = nullptr) : parent(parent) {}
  void insert(const std::string &name, const Value &val) {
    if (table.find(name) != table.end())
      throw redefinition_error(name);
    table[name] = val;
  }
  void clear() { table.clear(); }
  void insertConst(const std::string &name, int32_t val) {
    insert(name, {val, Value::Kind::CONST});
  }
  void insertVar(const std::string &name) {
    insert(name, {0, Value::Kind::VAR});
  }
  void insertFunc(const std::string &name) {
    insert(name, {0, Value::Kind::FUNC});
  }
  const Value &lookup(const std::string &name) const {
    auto it = table.find(name);
    if (it != table.end())
      return it->second;
    if (parent)
      return parent->lookup(name);
    throw undeclared_error(name);
  }
  int32_t lookupConst(const std::string &name) const {
    const Value &val = lookup(name);
    if (val.kind != Value::Kind::CONST)
      throw not_const_error(name);
    return val.val;
  }
  friend std::ostream &operator<<(std::ostream &os, const SymbolTable &symtab) {
    for (const auto &entry : symtab.table) {
      os << entry.first << " = ";
      switch (entry.second.kind) {
      case Value::Kind::CONST:
        os << entry.second.val;
        break;
      case Value::Kind::VAR:
        os << "(var)";
        break;
      case Value::Kind::FUNC:
        os << "(func)";
        break;
      }
      os << std::endl;
    }
    return os;
  }
};

#endif // FLEA_SYM_HPP_FLAG
