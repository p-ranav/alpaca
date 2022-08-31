#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize enum class" * test_suite("enum_class")) {
  struct my_struct {
    enum class color { red, blue, green };
    color value;
  };

  my_struct s{my_struct::color::blue};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(1));
}