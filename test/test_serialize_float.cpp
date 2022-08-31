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

TEST_CASE("Serialize float" * test_suite("float")) {
  struct my_struct {
    float value;
  };

  my_struct s{3.14f};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 4);

  CONSTRUCT_EXPECTED_VALUE(float, s.value);
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    REQUIRE(bytes[i] == expected[i]);
  }
}

TEST_CASE("Serialize double" * test_suite("float")) {
  struct my_struct {
    double value;
  };

  my_struct s{2.71828};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 8);

  CONSTRUCT_EXPECTED_VALUE(double, s.value);
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    REQUIRE(bytes[i] == expected[i]);
  }
}