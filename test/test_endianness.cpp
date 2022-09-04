#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#if defined(__APPLE__) || defined(__MACH__)

TEST_CASE("MacOS is big endian" * test_suite("endian")) {
  REQUIRE(is_system_big_endian());
  REQUIRE(__ALPACA_BYTE_ORDER == __ALPACA_BIG_ENDIAN);

  uint16_t value = 5;
  std::vector<uint8_t> bytes;
  std::size_t byte_index = 0;
  copy_bytes_in_range(value, bytes, byte_index);
  REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
  REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
}
#endif