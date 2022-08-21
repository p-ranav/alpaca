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

TEST_CASE("Serialize pair" * test_suite("pair")) {
  struct my_struct {
    std::pair<int, double> value;
  };

  my_struct s{std::make_pair(5, 3.14)};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 15);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::pair));
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::int32));
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::float64));

  // int
  {
    CONSTRUCT_EXPECTED_VALUE(int, 5);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[3 + i] == expected[i]);
    }
  }

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(double, 3.14);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[7 + i] == expected[i]);
    }
  }
}