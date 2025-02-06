#ifndef FLEA_ERR_CPP_FLAG
#define FLEA_ERR_CPP_FLAG

#include <exception>
#include <string>

class flea_compiler_error : public std::exception {
private:
  const std::string msg;

public:
  flea_compiler_error(const std::string &msg) : msg(msg) {}
  const char *what() const noexcept override { return msg.c_str(); }
};

class redefinition_error : public flea_compiler_error {
public:
  redefinition_error(const std::string &name)
      : flea_compiler_error("redefinition of '" + name + "'") {}
};

class undeclared_error : public flea_compiler_error {
public:
  undeclared_error(const std::string &name)
      : flea_compiler_error("undeclared identifier '" + name + "'") {}
};

class not_const_error : public flea_compiler_error {
public:
  not_const_error(const std::string &name)
      : flea_compiler_error("identifier '" + name + "' is not a constant") {}
};

class arithmetic_error : public flea_compiler_error {
public:
  arithmetic_error(const std::string &msg) : flea_compiler_error(msg) {}
};

#endif // FLEA_ERR_CPP_FLAG