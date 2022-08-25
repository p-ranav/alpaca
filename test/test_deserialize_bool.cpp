#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize bool (true)" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{true};
    bytes = serialize(s);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == true);
  }
}

TEST_CASE("Deserialize bool (true)" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{false};
    bytes = serialize(s);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == false);
  }
}