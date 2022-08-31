#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize tuple<int, float, bool> into array" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  my_struct s{std::make_tuple(5, 3.14, true, "Hello", 'i')};
  std::array<uint8_t, 15> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 13);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[1 + i] == expected[i]);
    }
  }

  // bool
  REQUIRE(bytes[5] == static_cast<uint8_t>(true));

  // string size
  REQUIRE(bytes[6] == static_cast<uint8_t>(5));

  // string value
  REQUIRE(bytes[7] == static_cast<uint8_t>('H'));
  REQUIRE(bytes[8] == static_cast<uint8_t>('e'));
  REQUIRE(bytes[9] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[10] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[11] == static_cast<uint8_t>('o'));

  // char
  REQUIRE(bytes[12] == static_cast<uint8_t>('i'));
}