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
  REQUIRE(bytes.size() == 17);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::tuple));

  // int
  {
    CONSTRUCT_EXPECTED_VALUE(int, 5);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[1 + i] == expected[i]);
    }
  }

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[5 + i] == expected[i]);
    }
  }

  // bool
  REQUIRE(bytes[9] == static_cast<uint8_t>(true));

  // string size
  REQUIRE(bytes[10] == static_cast<uint8_t>(5));

  // string value
  REQUIRE(bytes[11] == static_cast<uint8_t>('H'));
  REQUIRE(bytes[12] == static_cast<uint8_t>('e'));
  REQUIRE(bytes[13] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[14] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[15] == static_cast<uint8_t>('o'));

  // char
  REQUIRE(bytes[16] == static_cast<uint8_t>('i'));
}