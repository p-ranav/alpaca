#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize chrono duration" * test_suite("chrono_duration")) {
  struct my_struct {
    std::chrono::milliseconds period;
  };

  my_struct s{std::chrono::milliseconds{500}};

  std::vector<uint8_t> bytes;
  auto bytes_written = serialize(s, bytes);

  REQUIRE(bytes_written == 3);
  REQUIRE(bytes[0] == static_cast<uint8_t>(0x74));
  REQUIRE(bytes[1] == static_cast<uint8_t>(0xf4));
  REQUIRE(bytes[2] == static_cast<uint8_t>(0x03));
}