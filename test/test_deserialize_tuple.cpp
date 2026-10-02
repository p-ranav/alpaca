#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize a top-level std::tuple" * test_suite("tuple")) {
  std::tuple<int, double> t(1, 2.3);
  std::vector<uint8_t> bytes;
  serialize(t, bytes);

  std::error_code ec;
  auto result = deserialize<std::tuple<int, double>>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(result) == 1);
  REQUIRE(std::get<1>(result) == 2.3);
}

TEST_CASE("Deserialize a struct with a single-element std::tuple member" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<int> t{1};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(result.t) == 1);
}

TEST_CASE("Deserialize tuple<int, float, bool>" * test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  std::vector<uint8_t> bytes;
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

  std::vector<uint8_t> bytes;
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

TEST_CASE("Deserialize a top-level single-element std::tuple" *
          test_suite("tuple")) {
  std::tuple<int> t(42);
  std::vector<uint8_t> bytes;
  serialize(t, bytes);

  std::error_code ec;
  auto result = deserialize<std::tuple<int>>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(result) == 42);
}

TEST_CASE("Deserialize single-element std::tuple members of various types" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<std::string> a{"hello"};
    std::tuple<bool> b{true};
    std::tuple<float> c{3.14f};
    std::tuple<std::vector<int>> d{std::vector<int>{1, 2, 3}};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(result.a) == "hello");
  REQUIRE(std::get<0>(result.b) == true);
  REQUIRE(std::get<0>(result.c) == 3.14f);
  REQUIRE((std::get<0>(result.d) == std::vector<int>{1, 2, 3}));
}

TEST_CASE("Deserialize a nested tuple (tuple containing a tuple)" *
          test_suite("tuple")) {
  std::tuple<std::tuple<int, double>, float> t{std::make_tuple(1, 2.5), 3.5f};
  std::vector<uint8_t> bytes;
  serialize(t, bytes);

  std::error_code ec;
  auto result =
      deserialize<std::tuple<std::tuple<int, double>, float>>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(std::get<0>(result)) == 1);
  REQUIRE(std::get<1>(std::get<0>(result)) == 2.5);
  REQUIRE(std::get<1>(result) == 3.5f);
}

TEST_CASE(
    "Deserialize a single-element tuple containing a single-element tuple" *
    test_suite("tuple")) {
  struct my_struct {
    std::tuple<std::tuple<int>> t{std::make_tuple(7)};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(std::get<0>(result.t)) == 7);
}

TEST_CASE("Deserialize a struct mixing plain fields with single- and "
          "multi-element tuple members" *
          test_suite("tuple")) {
  struct my_struct {
    int a;
    std::tuple<int> b{10};
    std::tuple<int, double> c{11, 12.5};
    float d;
    std::tuple<std::string> e{"world"};
  };

  my_struct s{1, {10}, {11, 12.5}, 2.5f, {"world"}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.a == 1);
  REQUIRE(std::get<0>(result.b) == 10);
  REQUIRE(std::get<0>(result.c) == 11);
  REQUIRE(std::get<1>(result.c) == 12.5);
  REQUIRE(result.d == 2.5f);
  REQUIRE(std::get<0>(result.e) == "world");
}

TEST_CASE("Deserialize a large top-level tuple" * test_suite("tuple")) {
  std::tuple<int, int, int, int, int, int, int, int, int, int> t(
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  std::vector<uint8_t> bytes;
  serialize(t, bytes);

  std::error_code ec;
  using tuple_t = std::tuple<int, int, int, int, int, int, int, int, int, int>;
  auto result = deserialize<tuple_t>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(std::get<0>(result) == 1);
  REQUIRE(std::get<9>(result) == 10);
}

TEST_CASE("Deserialize a struct with an empty std::tuple<> member" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<> t{};
    int a;
  };

  my_struct s{{}, 5};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.a == 5);
}