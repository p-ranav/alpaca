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
  REQUIRE(bytes.size() == 7);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::int8));
  // size
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[3] == static_cast<uint8_t>(3));
  // values
  char current_value = 'x';
  for (std::size_t i = 4; i < bytes.size();) {
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
  REQUIRE(bytes.size() == 24);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::int32));
  // size
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[3] == static_cast<uint8_t>(5));
  // values
  int current_value = 1;
  for (std::size_t i = 4; i < bytes.size();) {
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
  REQUIRE(bytes.size() == 24);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::float32));
  // size
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[3] == static_cast<uint8_t>(5));
  // values
  float current_value = 1.1;
  for (std::size_t i = 4; i < bytes.size();) {
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
  REQUIRE(bytes.size() == 9);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::boolean));
  // size
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[3] == static_cast<uint8_t>(5));
  // values
  bool current_value = true;
  for (std::size_t i = 4; i < bytes.size();) {
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
  REQUIRE(bytes.size() == 19);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::string));
  // size
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[3] == static_cast<uint8_t>(5));
  // values
  char current_value = 'a';
  for (std::size_t i = 4; i < bytes.size();) {
    REQUIRE(bytes[i++] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
    REQUIRE(bytes[i++] == static_cast<uint8_t>(1));

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
  REQUIRE(bytes.size() == 33);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::vector));
  // value_type
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::vector));
  REQUIRE(bytes[2] == static_cast<uint8_t>(detail::type::int32));
  // size
  REQUIRE(bytes[3] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[4] == static_cast<uint8_t>(2));

  // first sub-vector
  REQUIRE(bytes[5] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[6] == static_cast<uint8_t>(3));

  // values
  int current_value = 1;
  for (std::size_t i = 7; i < bytes.size();) {
    CONSTRUCT_EXPECTED_VALUE(int32_t, current_value++);
    for (std::size_t j = 0; j < expected.size(); ++j) {
      REQUIRE(bytes[i++] == expected[j]);
    }

    if (current_value == 4) {
      // size of second sub-vector
      REQUIRE(bytes[i++] ==
              static_cast<uint8_t>(detail::type::uint64_as_uint8));
      REQUIRE(bytes[i++] == static_cast<uint8_t>(3));
    }
  }
}