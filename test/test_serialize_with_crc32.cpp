#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize int with crc32" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  my_struct s{5};
  std::vector<uint8_t> bytes;
  serialize<options::with_checksum>(s, bytes);

  REQUIRE(bytes.size() == 5);

  REQUIRE(bytes[0] == static_cast<uint32_t>(0x05));

  // serializes in little endian
  // 0x02 0x1b 0x68 0xa2
  REQUIRE(bytes[1] == static_cast<uint32_t>(0x02));
  REQUIRE(bytes[2] == static_cast<uint32_t>(0x1b));
  REQUIRE(bytes[3] == static_cast<uint32_t>(0x68));
  REQUIRE(bytes[4] == static_cast<uint32_t>(0xa2));
}