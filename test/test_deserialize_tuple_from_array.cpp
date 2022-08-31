#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize tuple<int, float, bool>" * test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  std::array<uint8_t, 32> bytes;
  {
    my_struct s{std::make_tuple(5, 3.14, true, "Hello", 'i')};
    serialize(s, bytes);
  }
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(std::get<0>(result.values) == 5);
    REQUIRE(std::get<1>(result.values) == 3.14f);
    REQUIRE(std::get<2>(result.values) == true);
    REQUIRE(std::get<3>(result.values) == std::string{"Hello"});
    REQUIRE(std::get<4>(result.values) == 'i');
  }
}

TEST_CASE("Deserialize tuple<std::vector<int>, std::vector<tuple>>" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<std::vector<int>, std::vector<std::tuple<int, float>>> values;
  };

  std::array<uint8_t, 32> bytes;
  {
    my_struct s{
        std::make_tuple(std::vector<int>{1, 2, 3},
                        std::vector<std::tuple<int, float>>{
                            std::make_tuple(4, 5.5), std::make_tuple(6, 7.7)})};
    serialize(s, bytes);
  }
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((std::get<0>(result.values) == std::vector<int>{1, 2, 3}));
    REQUIRE((std::get<1>(result.values) ==
             std::vector<std::tuple<int, float>>{std::make_tuple(4, 5.5),
                                                 std::make_tuple(6, 7.7)}));
  }
}