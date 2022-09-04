#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize optional<int>" * test_suite("optional")) {
  struct my_struct {
    std::optional<int> value;
  };

#if !defined(_MSC_VER) && !defined(__MINGW32__)
  {
    my_struct s{5};
    std::vector<uint8_t> bytes;
    serialize(s, bytes);
    REQUIRE(bytes.size() == 0); // fail, number of fields in struct deduced as 0
  }
#endif

  {
    my_struct s{5};
    std::vector<uint8_t> bytes;
    serialize<my_struct, 1>(s, bytes); // number of fields in struct specified
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(true)); // has_value
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));    // value = 5
  }

  {
    my_struct s{};
    std::vector<uint8_t> bytes;
    serialize<my_struct, 1>(s, bytes);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }

  {
    my_struct s{std::nullopt};
    std::vector<uint8_t> bytes;
    serialize<my_struct, 1>(s, bytes);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }
}