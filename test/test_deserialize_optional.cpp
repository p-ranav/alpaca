#include <doctest.hpp>
#include <structbyte/structbyte.h>
using namespace structbyte;

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

TEST_CASE("Deserialize optional sandwiched by regular types" *
          test_suite("optional")) {
  struct my_struct {
    bool before;
    std::optional<int> value;
    float after;
  };

  {
    std::vector<uint8_t> bytes;
    {
      my_struct s{true, 5, 3.14f};
      // number of fields in struct needs to be specified
      bytes = serialize<my_struct, 3>(s);
      REQUIRE(bytes.size() == 7);
    }
    {
      // number of fields in struct needs to be specified
      auto result = deserialize<my_struct, 3>(bytes);

      REQUIRE(result.before == true);
      REQUIRE(result.value.has_value() == true);
      REQUIRE(result.value.value() == 5);
      REQUIRE(result.after == 3.14f);
    }
  }
}