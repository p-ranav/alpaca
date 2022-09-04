#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(value)                                        \
  std::vector<uint8_t> expected;                                               \
  std::size_t index = 0;                                                       \
  detail::encode_varint<decltype(value), std::vector<uint8_t>>(                \
      value, expected, index);

TEST_CASE("Serialize uint8_t into array" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t value;
  };

  my_struct s{5};
  std::array<uint8_t, 20> bytes;
  auto bytes_written = serialize(s, bytes);
  REQUIRE(bytes_written == 1);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize uint16_t into array" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  {
    my_struct s{5};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);

    // serializes in little endian

    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x00));
  }

  {
    my_struct s{12345};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);

    // serializes in little endian

    REQUIRE(bytes[0] == static_cast<uint8_t>(0x39));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x30));
  }
}

TEST_CASE("Serialize uint32_t into array" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  {
    my_struct s{5};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes_written - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }

  {
    my_struct s{12345678};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 4);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes_written - 1; ++i) {
      REQUIRE(bytes[i] == expected[i]);
    }
  }
}

TEST_CASE("Serialize uint64_t into array" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  {
    my_struct s{5};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes_written - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }

  {
    my_struct s{12345678};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 4);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes_written - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }

  {
    my_struct s{5294967295};
    std::array<uint8_t, 20> bytes;
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 5);

    CONSTRUCT_EXPECTED_VALUE(s.value);
    for (std::size_t i = 0; i < bytes_written - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 0]);
    }
  }
}