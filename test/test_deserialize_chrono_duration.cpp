#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize chrono duration" * test_suite("chrono_duration")) {
  struct my_struct {
    std::chrono::milliseconds period;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{std::chrono::milliseconds{500}};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto object = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(object.period.count() == 500);
  }
}