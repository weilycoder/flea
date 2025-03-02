#ifndef FLEA_SYM_HPP_FLAG
#define FLEA_SYM_HPP_FLAG

#include "flea_err.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>

using Value = std::variant<int32_t, uint32_t>;

class SymbolTable {
protected:
  uint32_t offset = 0;
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
  void insertConst(const std::string &name, int32_t val) { insert(name, val); }
  void insertVar(const std::string &name) { insert(name, ++offset); }
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
    try {
      return std::get<int32_t>(val);
    } catch (const std::bad_variant_access &) {
      throw not_const_error(name);
    }
  }
  friend std::ostream &operator<<(std::ostream &os, const SymbolTable &symtab) {
    auto visitor = [&os](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, int32_t>)
        os << "(i32) " << arg;
      else if constexpr (std::is_same_v<T, uint32_t>)
        os << "(var) %" << arg;
      else
        static_assert(false, "non-exhaustive visitor!");
    };
    for (const auto &entry : symtab.table) {
      os << entry.first << " : ";
      std::visit(visitor, entry.second);
      os << std::endl;
    }
    return os;
  }
};

#endif // FLEA_SYM_HPP_FLAG
