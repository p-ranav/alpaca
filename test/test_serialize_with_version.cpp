#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize int with struct version" * test_suite("version")) {
  struct my_struct {
    int value;
  };

  my_struct s{5};
  auto bytes = serialize<my_struct, options::with_version>(s);

  REQUIRE(bytes.size() == 5);

  if (detail::is_system_little_endian()) {
    REQUIRE(bytes[0] == static_cast<uint32_t>(0x13));
    REQUIRE(bytes[1] == static_cast<uint32_t>(0xfe));
    REQUIRE(bytes[2] == static_cast<uint32_t>(0x34));
    REQUIRE(bytes[3] == static_cast<uint32_t>(0x94));
  } else {
    REQUIRE(bytes[0] == static_cast<uint32_t>(0x94));
    REQUIRE(bytes[1] == static_cast<uint32_t>(0x34));
    REQUIRE(bytes[2] == static_cast<uint32_t>(0xfe));
    REQUIRE(bytes[3] == static_cast<uint32_t>(0x13));
  }
  REQUIRE(bytes[4] == static_cast<uint32_t>(0x05));
}