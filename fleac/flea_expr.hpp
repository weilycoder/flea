#ifndef FLEA_COM_CPP_FLAG
#define FLEA_COM_CPP_FLAG

#include "flea_err.hpp"
#include <cstdint>

std::string id2tp(char id);
std::string id2op(char id);
int32_t safe_add(int32_t a, int32_t b);
int32_t safe_sub(int32_t a, int32_t b);
int32_t safe_mul(int32_t a, int32_t b);
int32_t floor_div(int32_t a, int32_t b);
int32_t floor_mod(int32_t a, int32_t b);

#endif // FLEA_COM_CPP_FLAG