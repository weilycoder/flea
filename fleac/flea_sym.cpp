#include "flea_sym.hpp"
#include "flea_err.hpp"
#include "flea_expr.hpp"

FuncSign::FuncSign(char returnType, std::vector<char> argTypes)
    : returnType(returnType), argTypes(argTypes) {}
bool FuncSign::operator==(const FuncSign &other) const {
  return returnType == other.returnType && argTypes == other.argTypes;
}

SymbolTable::SymbolTable(const SymbolTable *parent)
    : offset(parent ? parent->offset : 0), parent(parent) {}

void SymbolTable::clear() { table.clear(); }
void SymbolTable::insert(const std::string &name, const Value &val) {
  if (table.find(name) != table.end())
    throw redefinition_error(name);
  table[name] = val;
}
void SymbolTable::insertConst(const std::string &name, int32_t val) {
  insert(name, val);
}
void SymbolTable::insertFunc(const std::string &name, const FuncSign &val) {
  insert(name, val);
}
void SymbolTable::insertVar(const std::string &name) { insert(name, offset++); }
const Value &SymbolTable::lookup(const std::string &name) const {
  auto it = table.find(name);
  if (it != table.end())
    return it->second;
  if (parent)
    return parent->lookup(name);
  throw undeclared_error(name);
}
FuncSign SymbolTable::lookupFunc(const std::string &name) const {
  const Value &val = lookup(name);
  try {
    return std::get<FuncSign>(val);
  } catch (const std::bad_variant_access &) {
    throw not_func_error(name);
  }
}
int32_t SymbolTable::lookupConst(const std::string &name) const {
  const Value &val = lookup(name);
  try {
    return std::get<int32_t>(val);
  } catch (const std::bad_variant_access &) {
    throw not_const_error(name);
  }
}
std::ostream &operator<<(std::ostream &os, const SymbolTable &symtab) {
  auto visitor = [&os](auto &&arg) {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int32_t>)
      os << "(i32) " << arg;
    else if constexpr (std::is_same_v<T, int64_t>)
      os << "(var) %" << arg;
    else if constexpr (std::is_same_v<T, FuncSign>) {
      os << "(func) ";
      os << arg.returnType << " (";
      for (size_t i = 0; i < arg.argTypes.size(); i++) {
        if (i > 0)
          os << ", ";
        os << arg.argTypes[i];
      }
      os << ")";
    } else
      static_assert(false, "non-exhaustive visitor!");
  };
  for (const auto &entry : symtab.table) {
    os << entry.first << " : ";
    std::visit(visitor, entry.second);
    os << std::endl;
  }
  return os;
}
