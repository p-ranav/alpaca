#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize optional<int> into array" * test_suite("optional")) {
  struct my_struct {
    std::optional<int> value;
  };

#if !defined(_MSC_VER) && !defined(__MINGW32__)
  {
    my_struct s{5};
    std::array<uint8_t, 5> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written ==
            0); // fail, number of fields in struct deduced as 0
  }
#endif

  {
    my_struct s{5};
    std::array<uint8_t, 5> bytes;
    auto bytes_written = serialize<my_struct, 1>(
        s, bytes); // number of fields in struct specified
    REQUIRE(bytes_written == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(true)); // has_value
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));    // value = 5
  }

  {
    my_struct s{};
    std::array<uint8_t, 5> bytes;
    auto bytes_written = serialize<my_struct, 1>(s, bytes);
    REQUIRE(bytes_written == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }

  {
    my_struct s{std::nullopt};
    std::array<uint8_t, 5> bytes;
    auto bytes_written = serialize<my_struct, 1>(s, bytes);
    REQUIRE(bytes_written == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }
}