#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize deque<char>" * test_suite("vector")) {
  struct my_struct {
    std::deque<char> value;
  };

  my_struct s{{'x', 'y', 'z'}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 4);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(3));
  // values
  char current_value = 'x';
  for (std::size_t i = 1; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(char, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize deque<uint64_t>" * test_suite("vector")) {
  struct my_struct {
    std::deque<uint64_t> value;
  };

  my_struct s{{1, 2, 3, 4, 5}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 6);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // values
  REQUIRE(bytes[1] == static_cast<uint8_t>(1));
  REQUIRE(bytes[2] == static_cast<uint8_t>(2));
  REQUIRE(bytes[3] == static_cast<uint8_t>(3));
  REQUIRE(bytes[4] == static_cast<uint8_t>(4));
  REQUIRE(bytes[5] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize deque<int>" * test_suite("vector")) {
  struct my_struct {
    std::deque<int> value;
  };

  my_struct s{{1, 2, 3, 4, 5}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 6);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // values
  REQUIRE(bytes[1] == static_cast<uint8_t>(1));
  REQUIRE(bytes[2] == static_cast<uint8_t>(2));
  REQUIRE(bytes[3] == static_cast<uint8_t>(3));
  REQUIRE(bytes[4] == static_cast<uint8_t>(4));
  REQUIRE(bytes[5] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize deque<float>" * test_suite("vector")) {
  struct my_struct {
    std::deque<float> value;
  };

  my_struct s{{1.1, 2.2, 3.3, 4.4, 5.5}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 21);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // values
  float current_value = 1.1;
  for (std::size_t i = 1; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(float, current_value);
    current_value += 1.1;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

#ifndef __APPLE__
TEST_CASE("Serialize deque<bool>" * test_suite("vector")) {
  struct my_struct {
    std::deque<bool> value;
  };

  my_struct s{{true, false, true, false, true}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 6);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // values
  bool current_value = true;
  for (std::size_t i = 1; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(bool, current_value);
    current_value = !current_value;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}
#endif

TEST_CASE("Serialize deque<std::string>" * test_suite("vector")) {
  struct my_struct {
    std::deque<std::string> value;
  };

  my_struct s{{"a", "b", "c", "d", "e"}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 11);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  // values
  char current_value = 'a';
  for (std::size_t i = 1; i < bytes.size();) {
    REQUIRE(bytes[i++] == static_cast<uint8_t>(1));

    CONSTRUCT_EXPECTED_VALUE(char, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize deque<deque<int>>" * test_suite("vector")) {
  struct my_struct {
    std::deque<std::deque<int>> value;
  };

  my_struct s{{{1, 2, 3}, {4, 5, 6}}};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 9);
  // size
  REQUIRE(bytes[0] == static_cast<uint8_t>(2));

  // first sub-vector
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(3));
  // values
  REQUIRE(bytes[2] == static_cast<uint8_t>(1));
  REQUIRE(bytes[3] == static_cast<uint8_t>(2));
  REQUIRE(bytes[4] == static_cast<uint8_t>(3));

  // second sub-vector
  // size
  REQUIRE(bytes[5] == static_cast<uint8_t>(3));
  // values
  REQUIRE(bytes[6] == static_cast<uint8_t>(4));
  REQUIRE(bytes[7] == static_cast<uint8_t>(5));
  REQUIRE(bytes[8] == static_cast<uint8_t>(6));
}

TEST_CASE("Serialize deque<tuple>" * test_suite("vector")) {
  struct my_struct {
    std::deque<std::tuple<bool, int, float, std::string, char>> values;
  };

  my_struct s;
  s.values.push_back(std::make_tuple(true, 5, 3.14, "Hello", 'a'));
  s.values.push_back(std::make_tuple(false, -15, 2.718, "World", 'z'));
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  REQUIRE(bytes.size() == 27);
  // vector of tuple

  // vector size
  REQUIRE(bytes[0] == static_cast<uint8_t>(2));

  // start vector values

  // vector[0]
  REQUIRE(bytes[1] == static_cast<uint8_t>(true));
  REQUIRE(bytes[2] == static_cast<uint8_t>(5));
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[3 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[7] == static_cast<uint8_t>(5));
    REQUIRE(bytes[8] == static_cast<uint8_t>('H'));
    REQUIRE(bytes[9] == static_cast<uint8_t>('e'));
    REQUIRE(bytes[10] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[11] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[12] == static_cast<uint8_t>('o'));
  }
  REQUIRE(bytes[13] == static_cast<uint8_t>('a'));

  // vector[1]
  REQUIRE(bytes[14] == static_cast<uint8_t>(false));
  REQUIRE(bytes[15] == static_cast<uint8_t>(0b10001111));
  {
    CONSTRUCT_EXPECTED_VALUE(float, 2.718f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[16 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[20] == static_cast<uint8_t>(5));
    REQUIRE(bytes[21] == static_cast<uint8_t>('W'));
    REQUIRE(bytes[22] == static_cast<uint8_t>('o'));
    REQUIRE(bytes[23] == static_cast<uint8_t>('r'));
    REQUIRE(bytes[24] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[25] == static_cast<uint8_t>('d'));
  }
  REQUIRE(bytes[26] == static_cast<uint8_t>('z'));
}