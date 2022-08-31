#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize bool (true)" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  my_struct s{true};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(true));
}

TEST_CASE("Serialize bool (false)" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  my_struct s{false};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(false));
}