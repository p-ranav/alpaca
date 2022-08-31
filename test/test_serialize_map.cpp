#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize map<char, int>" * test_suite("map")) {
  struct my_struct {
    std::map<char, int> value;
  };

  my_struct s{{{'x', 1}, {'y', 2}, {'z', 3}}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 7);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(3));
  // key-value pairs
  REQUIRE(bytes[1] == static_cast<uint8_t>('x'));
  REQUIRE(bytes[2] == static_cast<uint8_t>(1));
  REQUIRE(bytes[3] == static_cast<uint8_t>('y'));
  REQUIRE(bytes[4] == static_cast<uint8_t>(2));
  REQUIRE(bytes[5] == static_cast<uint8_t>('z'));
  REQUIRE(bytes[6] == static_cast<uint8_t>(3));
}

TEST_CASE("Serialize map<std::string, vector<int>>" * test_suite("map")) {
  struct my_struct {
    std::map<std::string, std::vector<int>> value;
  };

  my_struct s{{{"time", {0, 1, 2, 3, 4}}, {"x", {5, 10, 15}}}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 18);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(2));

  // first key
  REQUIRE(bytes[1] == static_cast<uint8_t>(4));
  REQUIRE(bytes[2] == static_cast<uint8_t>('t'));
  REQUIRE(bytes[3] == static_cast<uint8_t>('i'));
  REQUIRE(bytes[4] == static_cast<uint8_t>('m'));
  REQUIRE(bytes[5] == static_cast<uint8_t>('e'));

  // first value
  // vector size
  REQUIRE(bytes[6] == static_cast<uint8_t>(5));
  // vector values
  REQUIRE(bytes[7] == static_cast<uint8_t>(0));
  REQUIRE(bytes[8] == static_cast<uint8_t>(1));
  REQUIRE(bytes[9] == static_cast<uint8_t>(2));
  REQUIRE(bytes[10] == static_cast<uint8_t>(3));
  REQUIRE(bytes[11] == static_cast<uint8_t>(4));

  // next key
  REQUIRE(bytes[12] == static_cast<uint8_t>(1));
  REQUIRE(bytes[13] == static_cast<uint8_t>('x'));

  // next value
  // vector size
  REQUIRE(bytes[14] == static_cast<uint8_t>(3));
  // vector values
  REQUIRE(bytes[15] == static_cast<uint8_t>(5));
  REQUIRE(bytes[16] == static_cast<uint8_t>(10));
  REQUIRE(bytes[17] == static_cast<uint8_t>(15));
}