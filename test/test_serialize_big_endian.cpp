#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize uint8_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    uint8_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
  }
}

TEST_CASE("Serialize uint16_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    uint16_t value;
  };

  // serialize
  {
    my_struct s{5};
    std::vector<uint8_t> bytes;
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
  }

  // serialize
  {
    my_struct s{12345};
    std::vector<uint8_t> bytes;
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x30));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x39));
  }
}

TEST_CASE("Serialize positive int8_t big endian" *
          test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
  }
}

TEST_CASE("Serialize positive int16_t big endian" *
          test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  // serialize
  {
    my_struct s{5};
    std::vector<uint8_t> bytes;
    serialize<options::big_endian>(s, bytes);
    // fixed width encoding
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
  }

  // serialize
  {
    my_struct s{12345};
    std::vector<uint8_t> bytes;
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
    // fixed width encoding
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x30));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x39));
  }
}

TEST_CASE("Serialize uint32_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{654321};
    serialize<options::big_endian | options::fixed_length_encoding>(s, bytes);
    REQUIRE(bytes.size() == 4);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x09));
    REQUIRE(bytes[2] == static_cast<uint8_t>(0xFB));
    REQUIRE(bytes[3] == static_cast<uint8_t>(0xF1));
  }
}