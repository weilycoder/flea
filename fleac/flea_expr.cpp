#include "flea_expr.hpp"
#include <cassert>

std::string id2tp(char id) {
  switch (id) {
  case static_cast<char>(VOID):
    return "void";
  case static_cast<char>(INT):
    return "int";
  default:
    assert(false);
    __builtin_unreachable();
  }
}

std::string id2op(char id) {
  switch (id) {
  case 0:
    return "null";
  case 'l':
    return "<=";
  case 'g':
    return ">=";
  case 'e':
    return "==";
  case 'n':
    return "!=";
  default:
    return std::string(1, id);
  }
}

int32_t safe_add(int32_t a, int32_t b) {
  return (int32_t)((uint32_t)a + (uint32_t)b);
}

int32_t safe_sub(int32_t a, int32_t b) {
  return (int32_t)((uint32_t)a - (uint32_t)b);
}

int32_t safe_mul(int32_t a, int32_t b) {
  return (int32_t)((uint32_t)a * (uint32_t)b);
}

int32_t floor_div(int32_t a, int32_t b) {
  if (b == 0)
    throw arithmetic_error("Division by zero");
  div_t r = div(a, b);
  return r.quot - (r.rem < 0 ? 1 : 0);
}

int32_t floor_mod(int32_t a, int32_t b) { return a - floor_div(a, b) * b; }
