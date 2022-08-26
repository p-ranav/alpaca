#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int with crc32" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    bytes = serialize(s, /*generate_crc = */ true);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec, /* check_crc = */ true);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}