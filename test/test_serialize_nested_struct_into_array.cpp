#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize nested struct (1 level) into array" *
          test_suite("struct")) {
  struct my_struct {
    int value;

    struct nested_struct {
      float value;
    };
    nested_struct nested;
  };

  my_struct s;
  s.value = 5;
  s.nested.value = 3.14f;

  std::array<uint8_t, 10> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 5);

  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // 3.14f
  REQUIRE(bytes[1] == static_cast<uint8_t>(0xc3));
  REQUIRE(bytes[2] == static_cast<uint8_t>(0xf5));
  REQUIRE(bytes[3] == static_cast<uint8_t>(0x48));
  REQUIRE(bytes[4] == static_cast<uint8_t>(0x40));
}

TEST_CASE("Serialize nested struct (2 levels) into array" *
          test_suite("struct")) {
  struct my_struct {
    int value;

    struct nested_struct {
      float value;

      struct nested_nested_struct {
        bool value;
      };
      nested_nested_struct nested;
    };
    nested_struct nested;
  };

  my_struct s;
  s.value = 5;
  s.nested.value = 3.14f;
  s.nested.nested.value = true;

  std::array<uint8_t, 10> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 6);

  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // 3.14f
  REQUIRE(bytes[1] == static_cast<uint8_t>(0xc3));
  REQUIRE(bytes[2] == static_cast<uint8_t>(0xf5));
  REQUIRE(bytes[3] == static_cast<uint8_t>(0x48));
  REQUIRE(bytes[4] == static_cast<uint8_t>(0x40));
  REQUIRE(bytes[5] == static_cast<uint8_t>(true));
}