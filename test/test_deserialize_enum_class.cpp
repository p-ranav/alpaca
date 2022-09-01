#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize enum class" * test_suite("enum_class")) {
  struct my_struct {
    enum class color { red, blue, green };
    color value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{my_struct::color::blue};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == my_struct::color::blue);
  }
}