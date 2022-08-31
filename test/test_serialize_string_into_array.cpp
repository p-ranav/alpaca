#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize std::string into array" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  my_struct s{"Hello World!"};
  std::array<uint8_t, 15> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 1 + s.value.size());
  REQUIRE(bytes[0] == static_cast<uint8_t>(12));
  // Value
  for (std::size_t i = 1; i < bytes_written; ++i) {
    REQUIRE(bytes[i] == static_cast<uint8_t>(s.value[i - 1]));
  }
}
