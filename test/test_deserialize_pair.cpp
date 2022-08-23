#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize pair" * test_suite("pair")) {
  struct my_struct {
    std::pair<int, double> value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{std::make_pair(5, 3.14)};
    bytes = serialize(s);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value.first == 5);
    REQUIRE(result.value.second == 3.14);
  }
}