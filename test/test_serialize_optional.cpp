#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize optional<int>" * test_suite("optional")) {
  struct my_struct {
    std::optional<int> value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 0); // fail, number of fields in struct deduced as 0
  }

  {
    my_struct s{5};
    auto bytes =
        serialize<my_struct, std::vector<uint8_t>, 1>(s); // number of fields in struct specified
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(true)); // has_value
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));    // value = 5
  }

  {
    my_struct s{};
    auto bytes = serialize<my_struct, std::vector<uint8_t>, 1>(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }

  {
    my_struct s{std::nullopt};
    auto bytes = serialize<my_struct, std::vector<uint8_t>, 1>(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(false));
  }
}