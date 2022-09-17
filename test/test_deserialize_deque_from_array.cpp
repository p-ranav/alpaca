#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize deque<char>" * test_suite("vector")) {
  struct my_struct {
    std::deque<char> values;
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
    REQUIRE((result.values == std::deque<char>{'x', 'y', 'z'}));
  }
}

TEST_CASE("Deserialize deque<size_t>" * test_suite("vector")) {
  struct my_struct {
    std::deque<uint64_t> values;
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
    REQUIRE((result.values == std::deque<uint64_t>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize deque<int>" * test_suite("vector")) {
  struct my_struct {
    std::deque<int> values;
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
    REQUIRE((result.values == std::deque<int>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Deserialize deque<deque<char>>" * test_suite("vector")) {
  struct my_struct {
    std::deque<std::deque<char>> values;
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
             std::deque<std::deque<char>>{{'a', 'b', 'c'}, {'d', 'e', 'f'}}));
  }
}

TEST_CASE("Deserialize deque<nested_struct>" * test_suite("vector")) {
  struct my_struct {
    std::deque<double> values;
    struct nested_t {
      int value;
    };
    std::deque<nested_t> nested_values;
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
    REQUIRE((result.values == std::deque<double>{1.23, 4.56, 7.89}));
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