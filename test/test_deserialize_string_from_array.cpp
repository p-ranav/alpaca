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

TEST_CASE("Deserialize std::string" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  std::array<uint8_t, 20> bytes;

  // serialize
  {
    my_struct s{"Hello World!"};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == std::string{"Hello World!"});
  }
}

TEST_CASE("Deserialize int and std::string" * test_suite("string")) {
  struct my_struct {
    int index;
    std::string value;
  };

  std::array<uint8_t, 20> bytes;

  // serialize
  {
    my_struct s{5, "Hello World!"};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.index == 5);
    REQUIRE(result.value == std::string{"Hello World!"});
  }
}