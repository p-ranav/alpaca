#include <doctest.hpp>
#include <structbyte/structbyte.h>
using namespace structbyte;

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(value)                                        \
  std::vector<uint8_t> expected;                                               \
  detail::encode_varint(value, expected);

TEST_CASE("Serialize uint8_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t value;
  };

  my_struct s{5};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize uint16_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}

TEST_CASE("Serialize uint32_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 4);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}

TEST_CASE("Serialize uint64_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 4);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }

  {
    my_struct s{5294967295};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }
}