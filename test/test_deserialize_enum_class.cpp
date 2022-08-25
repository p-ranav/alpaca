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
    bytes = serialize(s);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(result.value == my_struct::color::blue);
  }
}