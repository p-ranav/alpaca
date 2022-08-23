#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize optional<int>" * test_suite("optional")) {
  struct my_struct {
    std::optional<int> value;
  };

  {
    std::vector<uint8_t> bytes;
    {
      my_struct s{5};
      bytes =
          serialize<my_struct, 1>(s); // number of fields in struct specified
      REQUIRE(bytes.size() == 2);
    }
    {
      auto result = deserialize<my_struct>(
          bytes); // fail, number of fields in struct deduced as 0
      REQUIRE(result.value.has_value() == false);
    }
  }

  {
    std::vector<uint8_t> bytes;
    {
      my_struct s{5};
      bytes =
          serialize<my_struct, 1>(s); // number of fields in struct specified
      REQUIRE(bytes.size() == 2);
    }
    {
      auto result = deserialize<my_struct, 1>(
          bytes); // number of fields in struct specified
      REQUIRE(result.value.has_value() == true);
      REQUIRE(result.value.value() == 5);
    }
  }

  {
    std::vector<uint8_t> bytes;
    {
      my_struct s{};
      bytes =
          serialize<my_struct, 1>(s); // number of fields in struct specified
      REQUIRE(bytes.size() == 1);
    }
    {
      auto result = deserialize<my_struct, 1>(
          bytes); // number of fields in struct specified
      REQUIRE(result.value.has_value() == false);
    }
  }
}