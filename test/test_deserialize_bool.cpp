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
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
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
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == false);
  }
}