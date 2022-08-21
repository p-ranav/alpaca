#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize tuple<int, float, bool>" * test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  my_struct s{std::make_tuple(5, 3.14, true, "Hello", 'i')};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 25);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::tuple));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[2] == static_cast<uint8_t>(5));
  // type of each element
  REQUIRE(bytes[3] == static_cast<uint8_t>(detail::type::int32));
  REQUIRE(bytes[4] == static_cast<uint8_t>(detail::type::float32));
  REQUIRE(bytes[5] == static_cast<uint8_t>(detail::type::boolean));
  REQUIRE(bytes[6] == static_cast<uint8_t>(detail::type::string));
  REQUIRE(bytes[7] == static_cast<uint8_t>(detail::type::int8));

  // int
  {
    CONSTRUCT_EXPECTED_VALUE(int, 5);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[8 + i] == expected[i]);
    }
  }

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[12 + i] == expected[i]);
    }
  }

  // bool
  REQUIRE(bytes[16] == static_cast<uint8_t>(true));

  // string size
  REQUIRE(bytes[17] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[18] == static_cast<uint8_t>(5));

  // string value
  REQUIRE(bytes[19] == static_cast<uint8_t>('H'));
  REQUIRE(bytes[20] == static_cast<uint8_t>('e'));
  REQUIRE(bytes[21] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[22] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[23] == static_cast<uint8_t>('o'));

  // char
  REQUIRE(bytes[24] == static_cast<uint8_t>('i'));
}