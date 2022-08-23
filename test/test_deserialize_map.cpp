#include <doctest.hpp>
#include <map>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize map<char, int>" * test_suite("map")) {
  struct my_struct {
    std::map<char, int> value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{{'x', 1}, {'y', 2}, {'z', 3}}};
    bytes = serialize(s);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value.size() == 3);
    REQUIRE(result.value.at('x') == 1);
    REQUIRE(result.value.at('y') == 2);
    REQUIRE(result.value.at('z') == 3);
  }
}