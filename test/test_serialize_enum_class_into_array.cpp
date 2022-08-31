#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize enum class into array" * test_suite("enum_class")) {
  struct my_struct {
    enum class color { red, blue, green };
    color value;
  };

  my_struct s{my_struct::color::blue};
  std::array<uint8_t, 1> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(1));
}