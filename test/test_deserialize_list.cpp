#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize list<char>" * test_suite("vector")) {
  struct my_struct {
    std::list<char> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{'x', 'y', 'z'}};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 4);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::list<char>{'x', 'y', 'z'}));
  }
}

TEST_CASE("Deserialize list<size_t>" * test_suite("vector")) {
  struct my_struct {
    std::list<uint64_t> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 7);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::list<uint64_t>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize list<int>" * test_suite("vector")) {
  struct my_struct {
    std::list<int> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 7);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values == std::list<int>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize list<list<char>>" * test_suite("vector")) {
  struct my_struct {
    std::list<std::list<char>> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{{'a', 'b', 'c'}, {'d', 'e', 'f'}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.values ==
             std::list<std::list<char>>{{'a', 'b', 'c'}, {'d', 'e', 'f'}}));
  }
}

TEST_CASE("Deserialize list<nested_struct>" * test_suite("vector")) {
  struct my_struct {
    std::list<double> values;
    struct nested_t {
      int value;
    };
    std::list<nested_t> nested_values;
  };

  std::vector<uint8_t> bytes;

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
    REQUIRE((result.values == std::list<double>{1.23, 4.56, 7.89}));
    REQUIRE(result.nested_values.size() == 3);
    auto front = result.nested_values.front();
    REQUIRE(front.value == 1);
    result.nested_values.pop_front();
    front = result.nested_values.front();
    REQUIRE(front.value == 2);
    result.nested_values.pop_front();
    front = result.nested_values.front();
    REQUIRE(front.value == 3);
    result.nested_values.pop_front();
  }
}