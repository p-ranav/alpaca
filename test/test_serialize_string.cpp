#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize std::string" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  my_struct s{"Hello World!"};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 1 + s.value.size());
  REQUIRE(bytes[0] == static_cast<uint8_t>(12));
  // Value
  for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
    REQUIRE(bytes[i] == static_cast<uint8_t>(s.value[i - 1]));
  }
}
