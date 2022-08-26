#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize set<int> error - size value_too_large" *
          test_suite("vector")) {
  struct my_struct {
    std::set<int> values;
  };

  {
    auto bytes = std::vector<uint8_t>{0xff, 0x01, 0x02, 0x03};

    // size is 0xff which is too large since there are only 3 values
    // left in the buffer

    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::value_too_large));
  }
}