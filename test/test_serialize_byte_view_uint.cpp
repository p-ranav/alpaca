#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize uint32_t as byte_view" * test_suite("byte_view")) {
  struct my_struct {
    uint32_t value;
  };

  my_struct s{6553562};
  byte_view bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() ==
          sizeof(uint32_t)); // byte_view keeps raw size of each type
}