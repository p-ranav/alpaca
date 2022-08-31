#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize map<char, int>" * test_suite("map")) {
  struct my_struct {
    std::map<char, int> value;
  };

  std::array<uint8_t, 100> bytes;

  {
    my_struct s{{{'x', 1}, {'y', 2}, {'z', 3}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value.size() == 3);
    REQUIRE(result.value.at('x') == 1);
    REQUIRE(result.value.at('y') == 2);
    REQUIRE(result.value.at('z') == 3);
  }
}

TEST_CASE("Deserialize unordered_map<string, vector<int>>" *
          test_suite("map")) {
  struct my_struct {
    std::unordered_map<std::string, std::vector<int>> value;
  };

  std::array<uint8_t, 100> bytes;

  {
    my_struct s{{{"time", {0, 1, 2, 3, 4}}, {"x", {5, 10, 15}}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value.size() == 2);
    REQUIRE((result.value.at("time") == std::vector<int>{0, 1, 2, 3, 4}));
    REQUIRE((result.value.at("x") == std::vector<int>{5, 10, 15}));
  }
}

TEST_CASE("Deserialize unordered_map<string, vector<int>>" *
          test_suite("map")) {
  struct my_struct {
    std::unordered_map<std::string, std::map<int, double>> value;
  };

  std::array<uint8_t, 100> bytes;

  {
    my_struct s{{{"time", {{0, 1.1}, {1, 2.2}}}, {"x", {{3, 4.4}, {4, 5.5}}}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value.size() == 2);
    REQUIRE(
        (result.value.at("time") == std::map<int, double>{{0, 1.1}, {1, 2.2}}));
    REQUIRE(
        (result.value.at("x") == std::map<int, double>{{3, 4.4}, {4, 5.5}}));
  }
}