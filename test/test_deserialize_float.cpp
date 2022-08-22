#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize float" * test_suite("float")) {
  struct my_struct {
    float value;
  };

  std::vector<uint8_t> bytes;
  {
    my_struct s{3.14f};
    bytes = serialize(s);
  }
  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == 3.14f);
  }
}

TEST_CASE("Deserialize double" * test_suite("float")) {
  struct my_struct {
    double value;
  };

  std::vector<uint8_t> bytes;
  {
    my_struct s{2.71828};
    bytes = serialize(s);
  }
  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == 2.71828);
  }
}