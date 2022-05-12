//
// Created by Hedzr Yeh on 2021/2/10.
//


#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>


#include "cmdr11/cmdr_string.hh"


TEST_CASE("jaro-winkler test", "[text][edit][distance]") {

  using namespace cmdr::text;

  jaro_winkler_distance jaro{};

  struct test_case {
    const char *s1;
    const char *s2;
    Approx d;
  };
  using namespace Catch::literals;
  static test_case cases[] = {
      // {"TRATE", "TRACE", 0.906667_a},
      {"TRATE", "TRACE", 0.8666666667_a},
      {"CRATE", "TRACE", 0.73333333333333339_a},
      {"DwAyNE", "DuANE", 0.8222222222222223_a},
      {"developer", "developers", 0.96666666666666667_a},
      {"developer", "seveloper", 0.92592592592592593_a},
      {"mame", "name", 0.8333333333_a},
      {"mv", "mx", 0.6666666666666666_a},
      {"mv", "mx-test", 0.5476190476190476_a},
      {"mv", "micro-service", 0.5256410256_a},
      {"update-cc", "update-cv", 0.9259259259_a},
      {"AL", "AL", 1_a},
      {"MARTHA", "MARHTA", 0.9444444444_a},
      {"JONES", "JOHNSON", 0.7904761905_a},
      {"POTATO", "POTATTO", 0.9523809524_a},
      {"kitten", "sitting", 0.7460317460317460_a},
      {"MOUSE", "HOUSE", 0.8666666666666667_a},
  };

  for (const auto &case1: cases // {
                                // std::make_tuple(1, 2),
                                // std::make_tuple(2, 4),
                                // std::make_tuple(3, 6),
                                // std::make_tuple(4, 8),
                                //         }
  ) {
    SECTION("comparing: " + std::string(case1.s1) + std::string(" - ") + std::string(case1.s2)) {
      auto d = jaro(case1.s1, case1.s2);
      if (d != case1.d)
        std::cout << "expecting " << case1.d.toString() << ", but got " << d << '\n';
      CHECK(d == case1.d);
    }
  }

  // for (int i = 0; i < (int) countof(cases); i++) {
  //     SECTION("comparing s1 and s2") {
  //         auto d = jaro(cases[i].s1, cases[i].s2);
  //         REQUIRE(d == cases[i].d);
  //         // REQUIRE(is_double_eq(jaro(argv[0], argv[1]), 0.9002777778));
  //     }
  // }

  SECTION("developer - developers") {
    const char *argv[] = {"developer", "developers"};
    REQUIRE(jaro(argv[0], argv[1]) == Approx(0.96666666666666667));
    // REQUIRE(detail::is_double_eq(jaro(argv[0], argv[1]), 0.9002777778));
  }
  SECTION("developer - seveloper") {
    const char *argv[] = {"developer", "seveloper"};
    REQUIRE(jaro(argv[0], argv[1]) == 0.92592592592592593);
    // REQUIRE(detail::is_double_eq(jaro(argv[0], argv[1]), 0.92592592592592593));
  }
  SECTION("CRATE - TRACE") {
    const char *argv[] = {"CRATE", "TRACE"};
    REQUIRE(jaro(argv[0], argv[1]) == 0.73333333333333339);
    // REQUIRE(detail::is_double_eq(jaro(argv[0], argv[1]), 0.73333333333333339));
  }
  SECTION("DwAyNE - DuANE") {
    const char *argv[] = {"DwAyNE", "DuANE"};
    REQUIRE(jaro(argv[0], argv[1]) == 0.8222222222222223);
    // REQUIRE(detail::is_double_eq(jaro(argv[0], argv[1]), 0.8222222222222223));
  }
}
