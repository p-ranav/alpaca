#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize pair<int, double>" * test_suite("pair")) {
  struct my_struct {
    std::pair<int, double> value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{std::make_pair(5, 3.14)};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value.first == 5);
    REQUIRE(result.value.second == 3.14);
  }
}

TEST_CASE("Deserialize pair<std::vector<int>, std::vector<float>>" *
          test_suite("pair")) {
  struct my_struct {
    std::pair<std::vector<int>, std::vector<float>> value;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{std::make_pair(std::vector<int>{1, 2, 3},
                               std::vector<float>{1.1, 2.2, 3.3})};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.value.first == std::vector<int>{1, 2, 3}));
    REQUIRE(result.value.second == std::vector<float>{1.1, 2.2, 3.3});
  }
}