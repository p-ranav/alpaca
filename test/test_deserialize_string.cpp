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

TEST_CASE("Deserialize std::string" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{"Hello World!"};
    bytes = serialize(s);
  }

  // deserialize
  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == std::string{"Hello World!"});
  }
}