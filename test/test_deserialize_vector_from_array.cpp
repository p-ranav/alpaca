#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    std::vector<char> values;
  };

  std::array<uint8_t, 20> bytes;

  {
    my_struct s{{'x', 'y', 'z'}};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 4);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::vector<char>{'x', 'y', 'z'}));
  }
}

TEST_CASE("Deserialize vector<size_t>" * test_suite("vector")) {
  struct my_struct {
    std::vector<uint64_t> values;
  };

  std::array<uint8_t, 20> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 7);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::vector<uint64_t>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize vector<int>" * test_suite("vector")) {
  struct my_struct {
    std::vector<int> values;
  };

  std::array<uint8_t, 20> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 7);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::vector<int>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize vector<vector<char>>" * test_suite("vector")) {
  struct my_struct {
    std::vector<std::vector<char>> values;
  };

  std::array<uint8_t, 20> bytes;

  {
    my_struct s{{{'a', 'b', 'c'}, {'d', 'e', 'f'}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values ==
             std::vector<std::vector<char>>{{'a', 'b', 'c'}, {'d', 'e', 'f'}}));
  }
}

TEST_CASE("Deserialize vector<nested_struct>" * test_suite("vector")) {
  struct my_struct {
    std::vector<double> values;
    struct nested_t {
      int value;
    };
    std::vector<nested_t> nested_values;
  };

  std::array<uint8_t, 32> bytes;

  {
    my_struct s{{1.23, 4.56, 7.89},
                {my_struct::nested_t{1}, my_struct::nested_t{2},
                 my_struct::nested_t{3}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::vector<double>{1.23, 4.56, 7.89}));
    REQUIRE(result.nested_values.size() == 3);
    REQUIRE(result.nested_values[0].value == 1);
    REQUIRE(result.nested_values[1].value == 2);
    REQUIRE(result.nested_values[2].value == 3);
  }
}