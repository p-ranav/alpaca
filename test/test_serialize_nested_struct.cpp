#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Serialize nested struct (1 level)" * test_suite("struct")) {
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

  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 8);

  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int32_as_int8));
  REQUIRE(bytes[1] == static_cast<uint8_t>(5));

  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::struct_));
  REQUIRE(bytes[3] == static_cast<uint8_t>(detail::type::float32));
  // 3.14f
  REQUIRE(bytes[4] == static_cast<uint8_t>(0xc3));
  REQUIRE(bytes[5] == static_cast<uint8_t>(0xf5));
  REQUIRE(bytes[6] == static_cast<uint8_t>(0x48));
  REQUIRE(bytes[7] == static_cast<uint8_t>(0x40));
}

TEST_CASE("Serialize nested struct (2 levels)" * test_suite("struct")) {
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

  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 11);

  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int32_as_int8));
  REQUIRE(bytes[1] == static_cast<uint8_t>(5));

  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::struct_));
  REQUIRE(bytes[3] == static_cast<uint8_t>(detail::type::float32));
  // 3.14f
  REQUIRE(bytes[4] == static_cast<uint8_t>(0xc3));
  REQUIRE(bytes[5] == static_cast<uint8_t>(0xf5));
  REQUIRE(bytes[6] == static_cast<uint8_t>(0x48));
  REQUIRE(bytes[7] == static_cast<uint8_t>(0x40));

  REQUIRE(bytes[8] == static_cast<uint8_t>(detail::type::struct_));
  REQUIRE(bytes[9] == static_cast<uint8_t>(detail::type::boolean));
  REQUIRE(bytes[10] == static_cast<uint8_t>(true));
}