#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize set<int>" * test_suite("map")) {
  struct my_struct {
    std::set<int> value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value.size() == 4);
    REQUIRE(result.value.find(1) != result.value.end());
    REQUIRE(result.value.find(2) != result.value.end());
    REQUIRE(result.value.find(3) != result.value.end());
    REQUIRE(result.value.find(4) != result.value.end());
  }
}