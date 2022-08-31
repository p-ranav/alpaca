#include <alpaca/alpaca.h>
#include <doctest.hpp>
#include <set>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize set<int> into array" * test_suite("set")) {
  struct my_struct {
    std::set<int> value;
  };

  my_struct s{{1, 1, 1, 2, 3, 4}};
  std::array<uint8_t, 10> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 5);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(4));
  // values
  REQUIRE(bytes[1] == static_cast<uint8_t>(1));
  REQUIRE(bytes[2] == static_cast<uint8_t>(2));
  REQUIRE(bytes[3] == static_cast<uint8_t>(3));
  REQUIRE(bytes[4] == static_cast<uint8_t>(4));
}

TEST_CASE("Serialize unordered_set<int> into array" * test_suite("set")) {
  struct my_struct {
    std::unordered_set<int> value;
  };

  my_struct s{{4, 3, 2, 1, 1, 1, 1, 2, 3, 4}};
  std::array<uint8_t, 10> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 5);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(4));
  // values - order is unknown
  // deserialization will confirm that all the values are
  // correctly stored
}