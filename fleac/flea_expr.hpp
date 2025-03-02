#ifndef FLEA_COM_CPP_FLAG
#define FLEA_COM_CPP_FLAG

#include "flea_err.hpp"
#include <cstdint>
#include <cstdlib>

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

#endif // FLEA_COM_CPP_FLAG