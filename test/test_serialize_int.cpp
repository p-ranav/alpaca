#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE_SIGNED(type, value)                           \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  detail::encode_varint<type>(expected_value, expected);

TEST_CASE("Serialize int8_t" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  my_struct s{5};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize int16_t" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
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
    detail::print_bytes(bytes);
    REQUIRE(bytes.size() == 3);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int16_t, 12345);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}

TEST_CASE("Serialize int32_t" * test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
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
    REQUIRE(bytes.size() == 3);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int16_t, 12345);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int32_t, 12345678);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}

TEST_CASE("Serialize int64_t" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
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
    REQUIRE(bytes.size() == 3);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int16_t, 12345);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int32_t, 12345678);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }

  {
    my_struct s{5294967295};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 6);

    CONSTRUCT_EXPECTED_VALUE_SIGNED(int64_t, 5294967295);
    for (std::size_t i = 0; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}