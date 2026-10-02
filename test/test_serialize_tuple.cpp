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

TEST_CASE("Serialize a top-level std::tuple" * test_suite("tuple")) {
  std::tuple<int, double> t(1, 2.3);
  std::vector<uint8_t> bytes;
  auto n = serialize(t, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a struct with a single-element std::tuple member" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<int> t{1};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  auto n = serialize(s, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize tuple<int, float, bool>" * test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  my_struct s{std::make_tuple(5, 3.14, true, "Hello", 'i')};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);
  REQUIRE(bytes.size() == 13);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));

  // float
  {
    CONSTRUCT_EXPECTED_VALUE(float, 3.14f);
    for (std::size_t i = 0; i < expected.size(); ++i) {
      REQUIRE(bytes[1 + i] == expected[i]);
    }
  }

  // bool
  REQUIRE(bytes[5] == static_cast<uint8_t>(true));

  // string size
  REQUIRE(bytes[6] == static_cast<uint8_t>(5));

  // string value
  REQUIRE(bytes[7] == static_cast<uint8_t>('H'));
  REQUIRE(bytes[8] == static_cast<uint8_t>('e'));
  REQUIRE(bytes[9] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[10] == static_cast<uint8_t>('l'));
  REQUIRE(bytes[11] == static_cast<uint8_t>('o'));

  // char
  REQUIRE(bytes[12] == static_cast<uint8_t>('i'));
}

TEST_CASE("Serialize a top-level single-element std::tuple" *
          test_suite("tuple")) {
  std::tuple<int> t(42);
  std::vector<uint8_t> bytes;
  auto n = serialize(t, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize single-element std::tuple members of various types" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<std::string> a{"hello"};
    std::tuple<bool> b{true};
    std::tuple<float> c{3.14f};
    std::tuple<std::vector<int>> d{std::vector<int>{1, 2, 3}};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  auto n = serialize(s, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a nested tuple (tuple containing a tuple)" *
          test_suite("tuple")) {
  std::tuple<std::tuple<int, double>, float> t{std::make_tuple(1, 2.5), 3.5f};
  std::vector<uint8_t> bytes;
  auto n = serialize(t, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a single-element tuple containing a single-element tuple" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<std::tuple<int>> t{std::make_tuple(7)};
  };

  my_struct s{};
  std::vector<uint8_t> bytes;
  auto n = serialize(s, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a struct mixing plain fields with single- and "
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
  auto n = serialize(s, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a large top-level tuple" * test_suite("tuple")) {
  std::tuple<int, int, int, int, int, int, int, int, int, int> t(
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
  std::vector<uint8_t> bytes;
  auto n = serialize(t, bytes);
  REQUIRE(n > 0);
}

TEST_CASE("Serialize a struct with an empty std::tuple<> member" *
          test_suite("tuple")) {
  struct my_struct {
    std::tuple<> t{};
    int a;
  };

  my_struct s{{}, 5};
  std::vector<uint8_t> bytes;
  auto n = serialize(s, bytes);
  REQUIRE(n > 0);
}