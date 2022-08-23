#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    std::array<char, 3> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{'x', 'y', 'z'}};
    bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE((result.values == std::array<char, 3>{'x', 'y', 'z'}));
  }
}

TEST_CASE("Deserialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    std::array<std::array<int, 3>, 3> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}}};
    bytes = serialize(s);
    REQUIRE(bytes.size() == 9);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE((result.values == std::array<std::array<int, 3>, 3>{
                                  {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}}));
  }
}
