#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize bool (true) into array" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  my_struct s{true};
  std::array<uint8_t, 5> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(true));
}

TEST_CASE("Serialize bool (false) into array" * test_suite("bool")) {
  struct my_struct {
    bool value;
  };

  my_struct s{false};
  std::array<uint8_t, 5> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(false));
}