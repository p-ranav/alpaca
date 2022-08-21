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
    std::tuple<int, float, bool> values;
  };

  my_struct s{std::make_tuple(5, 3.14, true)};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 15);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::tuple));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[2] == static_cast<uint8_t>(3));
  // type of each element
  REQUIRE(bytes[3] == static_cast<uint8_t>(detail::type::int32));
  REQUIRE(bytes[4] == static_cast<uint8_t>(detail::type::float32));
  REQUIRE(bytes[5] == static_cast<uint8_t>(detail::type::boolean));

  // int
  {
    CONSTRUCT_EXPECTED_VALUE(int, 5);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[6 + i] == expected[i]);
    }
  }

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[10 + i] == expected[i]);
    }
  }

  // bool
  REQUIRE(bytes[14] == static_cast<uint8_t>(true));
}