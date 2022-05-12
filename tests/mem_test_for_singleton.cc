//
// Created by Hedzr Yeh on 2021/2/15.
//

// This file is a "Hello, world!" in C++ language by GCC for wandbox.
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>


#include <any>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <new>
#include <string>


#include <cstdio>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>


template<typename T>
class singleton {
public:
  static T &instance();

  singleton(const singleton &)          = delete;
  singleton &operator=(const singleton) = delete;

protected:
  struct token {};
  singleton() = default;
};

template<typename T>
inline T &singleton<T>::instance() {
  static const std::unique_ptr<T> instance{new T{token{}}};
  return *instance;
}


class app {
public:
  app() {
    std::cout << this << " | app::app()\n";
  }
  ~app() {
    std::cout << this << " | ~app::app()\n";
  }
  static app create() {
    app a;
    return a;
  }
  void use() {}
};

class app_holder final : public singleton<app_holder> {
public:
  explicit app_holder(typename singleton<app_holder>::token) {}
  ~app_holder() = default;

  [[maybe_unused]] void use() const { std::cout << "in use" << '\n'; }

public:
  app *get_ptr() { return _app; }
  app &operator*() { return *_app; }
  app *operator->() { return _app; }
  // explicit operator app() { return _app; }

private:
  app *_app{};
  void put(app *ptr) { _app = ptr; }
  friend class app;
};

inline app &get_app() { return *app_holder::instance().get_ptr(); }

inline app create_cli_app() {
  if (auto ptr = app_holder::instance().get_ptr(); ptr) {
    throw std::runtime_error("can't invoke create_app() multiple times.");
  }

  return app::create();
}

void test() {
  auto cli = create_cli_app();
  cli.use();
}

//

//
//

//


std::size_t allocated     = 0;
std::size_t released      = 0;
std::size_t released_size = 0;

void *operator new(size_t sz) {
  void *p = std::malloc(sz);
  allocated += sz;
  return p;
}

void operator delete(void *p) noexcept {
  released++;
  return std::free(p);
}

void operator delete(void *p, std::size_t sz) noexcept {
  released++;
  released_size += sz;
  return std::free(p);
}


//
//
//


int main() {
  std::cout << std::boolalpha;

  test();

  std::cout << "test() END.\n";

  std::printf("stack space = %zu, heap space = %zu (released: %zu)n",
              sizeof(get_app()), allocated, released);

  return 0;
}


// GCC reference:
//   https://gcc.gnu.org/

// C++ language references:
//   https://cppreference.com/
//   https://isocpp.org/
//   http://www.open-std.org/jtc1/sc22/wg21/

// Boost libraries references:
//   https://www.boost.org/doc/
