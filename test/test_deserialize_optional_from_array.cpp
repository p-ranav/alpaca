#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize optional<int>" * test_suite("optional")) {
  struct my_struct {
    std::optional<int> value;
  };

#if !defined(_MSC_VER) && !defined(__MINGW32__)
  {
    std::array<uint8_t, 10> bytes;
    {
      my_struct s{5};
      auto bytes_written = serialize<my_struct, 1>(
          s, bytes); // number of fields in struct specified
      REQUIRE(bytes_written == 2);
    }
    {
      std::error_code ec;
      auto result = deserialize<my_struct>(
          bytes, ec); // fail, number of fields in struct deduced as 0
      REQUIRE((bool)ec == false);
      REQUIRE(result.value.has_value() == false);
    }
  }
#endif

  {
    std::array<uint8_t, 10> bytes;
    {
      my_struct s{5};
      auto bytes_written = serialize<my_struct, 1>(
          s, bytes); // number of fields in struct specified
      REQUIRE(bytes_written == 2);
    }
    {
      std::error_code ec;
      auto result = deserialize<my_struct, 1>(
          bytes, ec); // number of fields in struct specified
      REQUIRE((bool)ec == false);
      REQUIRE(result.value.has_value() == true);
      REQUIRE(result.value.value() == 5);
    }
  }

  {
    std::array<uint8_t, 10> bytes;
    {
      my_struct s{};
      auto bytes_written = serialize<my_struct, 1>(
          s, bytes); // number of fields in struct specified
      REQUIRE(bytes_written == 1);
    }
    {
      std::error_code ec;
      auto result = deserialize<my_struct, 1>(
          bytes, ec); // number of fields in struct specified
      REQUIRE((bool)ec == false);
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
    std::array<uint8_t, 10> bytes;
    {
      my_struct s{true, 5, 3.14f};
      // number of fields in struct needs to be specified
      auto bytes_written = serialize<my_struct, 3>(s, bytes);
      REQUIRE(bytes_written == 7);
    }
    {
      // number of fields in struct needs to be specified
      std::error_code ec;
      auto result = deserialize<my_struct, 3>(bytes, ec);
      REQUIRE((bool)ec == false);
      REQUIRE(result.before == true);
      REQUIRE(result.value.has_value() == true);
      REQUIRE(result.value.value() == 5);
      REQUIRE(result.after == 3.14f);
    }
  }
}