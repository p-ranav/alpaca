#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    std::vector<char> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{'x', 'y', 'z'}};
    bytes = serialize(s);
    REQUIRE(bytes.size() == 4);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE((result.values == std::vector<char>{'x', 'y', 'z'}));
  }
}

TEST_CASE("Serialize vector<size_t>" * test_suite("vector")) {
  struct my_struct {
    std::vector<uint64_t> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    bytes = serialize(s);
    REQUIRE(bytes.size() == 7);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE((result.values == std::vector<uint64_t>{1, 2, 3, 4, 5, 6}));
  }
}

TEST_CASE("Serialize vector<int>" * test_suite("vector")) {
  struct my_struct {
    std::vector<int> values;
  };

  std::vector<uint8_t> bytes;

  {
    my_struct s{{1, 2, 3, 4, 5, 6}};
    bytes = serialize(s);
    REQUIRE(bytes.size() == 25);
  }

  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE((result.values == std::vector<int>{1, 2, 3, 4, 5, 6}));
  }
}