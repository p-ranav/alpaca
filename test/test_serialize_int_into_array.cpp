#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize int8_t into std::array" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  my_struct s{5};
  std::array<uint8_t, 1> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize int16_t into std::array" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  {
    my_struct s{5};
    std::array<uint8_t, 2> bytes;
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);

    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x00));
  }

  {
    my_struct s{5};
    std::array<uint8_t, 2> bytes;
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
  }

  {
    my_struct s{12345};
    std::array<uint8_t, 2> bytes;
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);

    REQUIRE(bytes[0] == static_cast<uint8_t>(0x39));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x30));
  }

  {
    my_struct s{12345};
    std::array<uint8_t, 2> bytes;
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x30));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x39));
  }
}