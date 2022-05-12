//
// Created by Hedzr Yeh on 2021/1/8.
//

#include <any>
#include <cassert>
#include <optional>
#include <utility>
#include <variant>


static void test1() {
  cmdr::Nullable<int> ii;
  CMDR_ASSERT(ii.is_null());
  ii.val(9);

  cmdr::Nullable<std::string> ss;
  ss.val("a string");

  std::optional<std::string> ov;
  std::cout << (int) ii << ',' << (std::string) ss << "||| " << ov << '\n';
}

static void ingest_any(const std::any &any) {
  try {
    std::cout << std::any_cast<std::string>(any) << '\n';
  } catch (std::bad_any_cast const &) {}

  if (std::string *str = std::any_cast<std::string>(&any)) {
    std::cout << *str << '\n';
  }

  if (std::type_index{typeid(std::string)} == any.type()) {
    //  Known not to throw, as previously checked.
    std::cout << std::any_cast<std::string>(any) << '\n';
  }
}

static void visit_any(const std::any &any) {
  // std::visit([](auto&& arg) {arg *= 2; }, v);
  std::visit([](auto &&arg) { std::cout << arg; }, any);
}
