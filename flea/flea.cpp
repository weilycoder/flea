#define time_limit 10000000
#define memory_size (1 << 23)

#include <conio.h>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

class interpreted_error : public exception {
private:
  const char *msg;

public:
  interpreted_error(const char *msg) : msg(msg) {}
  const char *what() const noexcept override { return msg; }
};

struct code;

int32_t line = -1;
vector<code> codes;
vector<string> code_id = {"=",  "+",  "-",    "*",    "/",    "<",
                          "==", "if", "getc", "geti", "putc", "puti"};
unordered_map<string, int32_t> code_names = {
    {"=", 0},  {"+", 1},  {"-", 2},    {"*", 3},    {"/", 4},     {"<", 5},
    {"==", 6}, {"if", 7}, {"getc", 8}, {"geti", 9}, {"putc", 10}, {"puti", 11}};

#ifdef DEBUG_MODE_ENABLED
bool breaking, break_all, fast_mode, interpret_debug;
#include <unordered_set>
unordered_set<int32_t> break_ln, break_val, break_mem, break_cmd;
#endif

int32_t memory[memory_size];

bool is_whitespace(char c) {
  switch (c) {
  case 0:
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

int32_t floor_div(int32_t a, int32_t b) {
  if (b == 0)
    throw interpreted_error("Division by zero");
  div_t r = div(a, b);
  return r.quot - (r.rem < 0 ? 1 : 0);
}

size_t check_tle() {
  static size_t cnt = 0;
  if (++cnt > time_limit)
    throw interpreted_error("Time limit exceeded");
  return cnt;
}

#ifdef EXITING_LOG_ENABLED
#include <ctime>
#include <iomanip>
static clock_t start_time = 0;
void output_exit_log(int code) {
  clock_t end_time = clock();
  clog << "====================" << endl;
  clog << "The program terminates after running ";
  clog << check_tle() << " step(s) ";
  clog << "with exit code " << code << "." << endl;
  clog << "Time elapsed: ";
  clog << fixed << setprecision(3);
  clog << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;
}
#endif

void exit_with_success(int code = EXIT_SUCCESS) {
#ifdef EXITING_LOG_ENABLED
  output_exit_log(code);
#endif
  exit(code);
}
void exit_with_error(const char *msg) {
  if (line == -1)
    clog << msg << endl;
  else
    clog << "Line " << line << ": " << msg << endl;
#ifdef EXITING_LOG_ENABLED
  output_exit_log(EXIT_FAILURE);
#endif
  exit(EXIT_FAILURE);
}

void sigint_handler(int) { exit_with_error("Interrupted by signal"); }

int32_t &get_val(int32_t addr) {
  if (0 <= addr && addr < memory_size)
    return memory[addr];
  throw interpreted_error("Invalid memory access");
  __builtin_unreachable();
}
int32_t &get_pointer(int32_t addr) { return get_val(get_val(addr)); }

int32_t to_int(const string &s) {
  try {
    return stol(s);
  } catch (invalid_argument &) {
    throw interpreted_error("Invalid integer");
  } catch (out_of_range &) {
    throw interpreted_error("Integer out of range");
  } catch (...) {
    throw interpreted_error("Unknown error");
  }
  __builtin_unreachable();
}

int32_t getc(istream &is = cin) { return is.get(); }
int32_t geti(istream &is = cin) {
  string s;
  return is >> s, to_int(s);
}
void putc(int32_t a, ostream &os = cout) {
  os.put((char)a);
#ifdef FLUSH_ENABLED
  os.flush();
#endif
}
void puti(int32_t a, ostream &os = cout) {
  os << a;
#ifdef FLUSH_ENABLED
  os.flush();
#endif
}

void go_to(int32_t a) {
  if (a == -1)
    exit_with_success();
  if (a < -1) [[unlikely]]
    exit_with_success(-1 - a);
  if (0 < a && a <= (int32_t)codes.size())
    line = a - 1;
  else
    throw interpreted_error("Invalid jump");
}

int32_t from_code_name(const string &s) {
  if (s.size() == 0)
    return -1;
  if (code_names.count(s))
    return code_names[s];
  throw interpreted_error("Invalid instruction");
  __builtin_unreachable();
}

struct value {
  char type;
  int32_t val;
  value() : type('@'), val(INT32_MAX) {}
  value(int32_t val) : type('@'), val(val) {}
  value(char type, int32_t val) : type(type), val(val) {
    switch (type) {
    case '@':
    case '$':
    case '#':
      break;
    default:
      throw interpreted_error("Invalid value type");
    }
  }
  int32_t get() const {
    switch (type) {
    case '@':
      return val;
    case '$':
      return get_val(val);
    case '#':
      return get_pointer(val);
    default:
      __builtin_unreachable();
    }
  }
  void set(const value &b) const {
    switch (type) {
    case '@':
      throw interpreted_error("Cannot assign to a constant");
    case '$':
      get_val(val) = b.get();
      break;
    case '#':
      get_pointer(val) = b.get();
      break;
    default:
      __builtin_unreachable();
    }
  }
#ifdef DEBUG_MODE_ENABLED
  pair<int32_t, int32_t> get_memory() const {
    switch (type) {
    case '@':
      return {-1, -1};
    case '$':
      return {val, -1};
    case '#':
      return {val, get_val(val)};
    default:
      __builtin_unreachable();
    }
  }
  bool check_memory(const unordered_set<int32_t> &addrs) const {
    auto [addr1, addr2] = get_memory();
    return addrs.count(addr1) || addrs.count(addr2);
  }
  bool check_value(const unordered_set<int32_t> &values) const {
    return values.count(get());
  }
#endif
  friend int32_t operator+(const value &a, const value &b) {
    return (int32_t)((uint32_t)a.get() + (uint32_t)b.get());
  }
  friend int32_t operator-(const value &a, const value &b) {
    return (int32_t)((uint32_t)a.get() - (uint32_t)b.get());
  }
  friend int32_t operator*(const value &a, const value &b) {
    return (int32_t)((uint32_t)a.get() * (uint32_t)b.get());
  }
  friend int32_t operator/(const value &a, const value &b) {
    return floor_div(a.get(), b.get());
  }
  friend int32_t operator<(const value &a, const value &b) {
    return a.get() < b.get() ? 1 : 0;
  }
  friend int32_t operator==(const value &a, const value &b) {
    return a.get() == b.get() ? 1 : 0;
  }
#ifdef DEBUG_MODE_ENABLED
  friend ostream &operator<<(ostream &os, const value &v) {
    os << v.type << v.val;
    switch (v.type) {
    case '@':
      return os;
    case '$':
      return os << "=" << get_val(v.val);
    case '#':
      return os << "(" << "$" << get_val(v.val) << ")=" << v.get();
    }
    __builtin_unreachable();
  }
#endif
  friend istream &operator>>(istream &is, value &v) {
    string s;
    is >> s;
    if (s.size() != 1) {
      if (s.size() != 0) {
        switch (s[0]) {
        case '@':
        case '$':
        case '#':
          v = value(s[0], to_int(s.substr(1)));
          return is;
        }
      }
      throw interpreted_error("Invalid value");
    }
    int32_t val = geti(is);
    v = value(s[0], val);
    return is;
  }
};

struct code {
  int32_t type;
  value a, b, c;
  code() : type(-1), a(), b(), c() {}
  code(int32_t type, value a, value b, value c) { set(type, a, b, c); }
  void set(int32_t type, value a, value b, value c) {
    this->type = type, this->a = a, this->b = b, this->c = c;
    switch (type) {
    case -1:
    case 0 ... 11:
      break;
    default:
      throw interpreted_error("Invalid instruction");
    }
  }
#ifdef DEBUG_MODE_ENABLED
  bool check_value(const unordered_set<int32_t> &values) const {
    return a.check_value(values) || b.check_value(values) ||
           c.check_value(values);
  }
  bool check_memory(const unordered_set<int32_t> &addrs) const {
    return a.check_memory(addrs) || b.check_memory(addrs) ||
           c.check_memory(addrs);
  }
  bool check_break() const {
    return breaking || break_all || break_ln.count(line) ||
           break_cmd.count(type) || check_value(break_val) ||
           check_memory(break_mem);
  }
#endif
  void execute() const {
#ifdef DEBUG_MODE_ENABLED
    if (check_break())
      clog << "Breakpoint at line " << line << ": " << *this << endl,
          breaking = true, interpret_debug = !fast_mode;
#endif
    switch (type) {
    case -1:
      [[unlikely]] throw interpreted_error("End of program");
    case 0:
      a.set(b);
      break;
    case 1:
      c.set(a + b);
      break;
    case 2:
      c.set(a - b);
      break;
    case 3:
      c.set(a * b);
      break;
    case 4:
      c.set(a / b);
      break;
    case 5:
      c.set(a < b);
      break;
    case 6:
      c.set(a == b);
      break;
    case 7:
      if (a.get())
        go_to(b.get());
      break;
    case 8:
      a.set(getc());
      break;
    case 9:
      a.set(geti());
      break;
    case 10:
      putc(a.get());
      break;
    case 11:
      puti(a.get());
      break;
    default:
      __builtin_unreachable();
    }
  }
#ifdef DEBUG_MODE_ENABLED
  friend ostream &operator<<(ostream &os, const code &c) {
    if (c.type == -1) [[unlikely]]
      return os;
    os << code_id[c.type] << " ";
    switch (c.type) {
    case 8 ... 11:
      return os << c.a;
    case 1 ... 6:
      return os << c.a << " " << c.b << " " << c.c;
    case 0:
    case 7:
      return os << c.a << " " << c.b;
    }
    __builtin_unreachable();
  }
#endif
  friend istream &operator>>(istream &is, code &c) {
    string type;
    is >> type;
    value x, y, z;
    int32_t tp = from_code_name(type);
    switch (tp) {
    case -1:
      c = code();
      break;
    case 8 ... 11:
      is >> x, c = code(tp, x, y, z);
      break;
    case 1 ... 6:
      is >> x >> y >> z, c = code(tp, x, y, z);
      break;
    case 0:
    case 7:
      is >> x >> y, c = code(tp, x, y, z);
      break;
    default:
      __builtin_unreachable();
    }
    return is;
  }
};

#ifdef DEBUG_MODE_ENABLED
void check_ln(int32_t ln) {
  if (ln < 1 || ln > (int32_t)codes.size())
    throw interpreted_error("Invalid line number");
}
ostream &print_mem(int32_t addr, ostream &os = clog) {
  if (addr == -1) {
    for (int32_t i = 0; i < memory_size; ++i)
      if (memory[i])
        print_mem(i, os) << endl;
    return os;
  } else {
    if (addr < 0 || addr >= memory_size)
      throw interpreted_error("Invalid memory address");
    int32_t val = memory[addr];
    os << "$" << addr << "=" << val;
    if (0 <= val && val < memory_size)
      os << ", #" << addr << "=" << memory[val];
    return os << ";";
  }
}
ostream &print_val(const value &v, ostream &os = clog) {
  return os << v.type << v.val;
}
ostream &print_code(const code &c, ostream &os = clog) {
  os << code_id[c.type] << " ";
  switch (c.type) {
  case 8 ... 11:
    return print_val(c.a, os);
  case 0:
  case 7:
    print_val(c.a, os) << " ";
    return print_val(c.b, os);
  case 1 ... 6:
    print_val(c.a, os) << " ";
    print_val(c.b, os) << " ";
    return print_val(c.c, os);
  default:
    __builtin_unreachable();
  }
}
void interpret_debug_cmd(const string &s) try {
  if (s.size() == 0)
    return;
  auto next = [](const string &s, size_t pos) -> string {
    while (pos < s.size() && is_whitespace(s[pos]))
      ++pos;
    return s.substr(pos);
  };
  int32_t tp;
  switch (s[0]) {
  case 'q':
  case 'Q':
    return exit_with_success();
  case 'f':
  case 'F':
    fast_mode = !fast_mode;
    [[fallthrough]];
  case 'r':
  case 'R':
    breaking = false;
    [[fallthrough]];
  case 's':
  case 'S':
    return interpret_debug = false, void();
  case 'b':
  case 'B':
    tp = 0;
    break;
  case 'd':
  case 'D':
    tp = 1;
    break;
  case 'p':
  case 'P':
    tp = 2;
    break;
  default:
    cerr << s << endl;
    throw interpreted_error("Invalid command");
  }
  string t = next(s, 1);
  if (t.size() == 0) {
    switch (tp) {
    case 0:
      break_all = true;
      break;
    case 1:
      break_all = false;
      break;
    case 2:
      print_mem(-1);
      break;
    default:
      __builtin_unreachable();
    }
  } else {
    switch (t[0]) {
    case '0' ... '9': {
      int32_t ln = to_int(t);
      check_ln(ln);
      switch (tp) {
      case 0:
        if (break_ln.count(ln))
          throw interpreted_error("Breakpoint already exists");
        break_ln.insert(ln);
        break;
      case 1:
        if (!break_ln.count(ln))
          throw interpreted_error("Breakpoint does not exist");
        break_ln.erase(ln);
        break;
      case 2:
        print_code(codes[ln - 1], clog << "Line " << ln << ": ");
        break;
      }
    } break;
    case '@': {
      int32_t val = to_int(next(t, 1));
      switch (tp) {
      case 0:
        if (break_val.count(val))
          throw interpreted_error("Breakpoint already exists");
        break_val.insert(val);
        break;
      case 1:
        if (!break_val.count(val))
          throw interpreted_error("Breakpoint does not exist");
        break_val.erase(val);
        break;
      case 2:
        throw interpreted_error("Invalid command");
      }
    } break;
    case '$': {
      int32_t val = to_int(next(t, 1));
      switch (tp) {
      case 0:
        if (break_mem.count(val))
          throw interpreted_error("Breakpoint already exists");
        break_mem.insert(val);
        break;
      case 1:
        if (!break_mem.count(val))
          throw interpreted_error("Breakpoint does not exist");
        break_mem.erase(val);
        break;
      case 2:
        print_mem(val) << endl;
        break;
      }
    } break;
    default: {
      int32_t c = from_code_name(t);
      switch (tp) {
      case 0:
        if (break_cmd.count(c))
          throw interpreted_error("Breakpoint already exists");
        break_cmd.insert(c);
        break;
      case 1:
        if (!break_cmd.count(c))
          throw interpreted_error("Breakpoint does not exist");
        break_cmd.erase(c);
        break;
      case 2:
        throw interpreted_error("Invalid command");
      }
    } break;
    }
  }
} catch (exception &e) {
  clog << "Warning: " << e.what() << endl;
}
#endif

int main(int argc, char *argv[]) try {
  signal(SIGINT, sigint_handler);
  cin.tie(nullptr)->sync_with_stdio(false);
  istream *ifs = nullptr;
#ifdef DEBUG_MODE_ENABLED
  istream *ids = nullptr;
#endif
  switch (argc) {
  case 1:
    ifs = &cin;
    break;
#ifdef DEBUG_MODE_ENABLED
  case 3:
    ids = new ifstream(argv[2]);
    [[fallthrough]];
#endif
  case 2:
    ifs = new ifstream(argv[1]);
    break;
  default:
    throw interpreted_error("Invalid arguments");
  }
  for (code c; *ifs >> c;)
    codes.push_back(c);
#ifdef COMPLIER_LOG_ENABLED
  clog << "Program loaded." << endl;
  clog << "Program size: " << codes.size() << endl;
#endif
#ifdef DEBUG_MODE_ENABLED
  if (ids) {
    interpret_debug = true;
    for (string s; *ids >> s;)
      interpret_debug_cmd(s);
    clog << "Debug mode enabled." << endl;
  }
#endif
  line = 1;
  cin.clear();
#ifdef EXITING_LOG_ENABLED
  start_time = clock();
#endif
  for (line = 1; line <= (int32_t)codes.size(); ++line, check_tle()) {
    codes[line - 1].execute();
#ifdef DEBUG_MODE_ENABLED
    if (interpret_debug) {
      clog << "====Debug====" << endl;
      while (interpret_debug) {
        string cmd;
        getline(cin, cmd);
        interpret_debug_cmd(cmd);
      }
      clog << "=====End=====" << endl;
    }
#endif
  }
  if (line > (int32_t)codes.size())
    throw interpreted_error("End of program");
  return 0;
} catch (exception &e) {
  exit_with_error(e.what());
}
