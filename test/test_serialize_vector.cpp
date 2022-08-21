#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    std::vector<char> value;
  };

  my_struct s{{'x', 'y', 'z'}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 5);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size3));
  // values
  char current_value = 'x';
  for (std::size_t i = 2; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(char, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize vector<int>" * test_suite("vector")) {
  struct my_struct {
    std::vector<int> value;
  };

  my_struct s{{1, 2, 3, 4, 5}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 22);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size5));
  // values
  int current_value = 1;
  for (std::size_t i = 2; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(int32_t, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize vector<float>" * test_suite("vector")) {
  struct my_struct {
    std::vector<float> value;
  };

  my_struct s{{1.1, 2.2, 3.3, 4.4, 5.5}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 22);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size5));
  // values
  float current_value = 1.1;
  for (std::size_t i = 2; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(float, current_value);
    current_value += 1.1;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize vector<bool>" * test_suite("vector")) {
  struct my_struct {
    std::vector<bool> value;
  };

  my_struct s{{true, false, true, false, true}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 7);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size5));
  // values
  bool current_value = true;
  for (std::size_t i = 2; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(bool, current_value);
    current_value = !current_value;
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize vector<std::string>" * test_suite("vector")) {
  struct my_struct {
    std::vector<std::string> value;
  };

  my_struct s{{"a", "b", "c", "d", "e"}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 12);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size5));
  // values
  char current_value = 'a';
  for (std::size_t i = 2; i < bytes.size();) {
    REQUIRE(bytes[i++] == static_cast<uint8_t>(detail::type::size1));

    CONSTRUCT_EXPECTED_VALUE(char, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }
  }
}

TEST_CASE("Serialize vector<vector<int>>" * test_suite("vector")) {
  struct my_struct {
    std::vector<std::vector<int>> value;
  };

  my_struct s{{{1, 2, 3}, {4, 5, 6}}};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 28);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size2));

  // first sub-vector
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::size3));

  // values
  int current_value = 1;
  for (std::size_t i = 3; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(int32_t, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }

    if (current_value == 4) {
      // size of second sub-vector
      REQUIRE(bytes[i++] == static_cast<uint8_t>(detail::type::size3));
    }
  }
}

TEST_CASE("Serialize vector<tuple>" * test_suite("vector")) {
  struct my_struct {
    std::vector<std::tuple<bool, int, float, std::string, char>> values;
  };

  my_struct s;
  s.values.push_back(std::make_tuple(true, 5, 3.14, "Hello", 'a'));
  s.values.push_back(std::make_tuple(false, -15, 2.718, "World", 'z'));
  auto bytes = serialize(s);

  REQUIRE(bytes.size() == 34);
  // vector of tuple
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));

  // vector size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size2));

  // start vector values

  // vector[0]
  REQUIRE(bytes[2] == static_cast<uint8_t>(true));
  {
    CONSTRUCT_EXPECTED_VALUE(int, 5);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[3 + i] == expected[i]);
    }
  }
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[7 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[11] == static_cast<uint8_t>(detail::type::size5));
    REQUIRE(bytes[12] == static_cast<uint8_t>('H'));
    REQUIRE(bytes[13] == static_cast<uint8_t>('e'));
    REQUIRE(bytes[14] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[15] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[16] == static_cast<uint8_t>('o'));
  }
  REQUIRE(bytes[17] == static_cast<uint8_t>('a'));

  // vector[1]
  REQUIRE(bytes[18] == static_cast<uint8_t>(false));
  {
    CONSTRUCT_EXPECTED_VALUE(int, -15);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[19 + i] == expected[i]);
    }
  }
  {
    CONSTRUCT_EXPECTED_VALUE(float, 2.718f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[23 + i] == expected[i]);
    }
  }
  {
    REQUIRE(bytes[27] == static_cast<uint8_t>(detail::type::size5));
    REQUIRE(bytes[28] == static_cast<uint8_t>('W'));
    REQUIRE(bytes[29] == static_cast<uint8_t>('o'));
    REQUIRE(bytes[30] == static_cast<uint8_t>('r'));
    REQUIRE(bytes[31] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[32] == static_cast<uint8_t>('d'));
  }
  REQUIRE(bytes[33] == static_cast<uint8_t>('z'));
}