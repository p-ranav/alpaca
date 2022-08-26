#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    int value;
  };

  my_struct s{5};
  auto bytes = serialize(s, /*generate_crc = */ true);

  REQUIRE(bytes.size() == 5);

  REQUIRE(bytes[0] == static_cast<uint32_t>(0x05));
  if (detail::is_system_little_endian()) {
    // 0x02 0x1b 0x68 0xa2
    REQUIRE(bytes[1] == static_cast<uint32_t>(0x02));
    REQUIRE(bytes[2] == static_cast<uint32_t>(0x1b));
    REQUIRE(bytes[3] == static_cast<uint32_t>(0x68));
    REQUIRE(bytes[4] == static_cast<uint32_t>(0xa2));
  } else {
    REQUIRE(bytes[1] == static_cast<uint32_t>(0xa2));
    REQUIRE(bytes[2] == static_cast<uint32_t>(0x68));
    REQUIRE(bytes[3] == static_cast<uint32_t>(0x1b));
    REQUIRE(bytes[4] == static_cast<uint32_t>(0x02));
  }
}