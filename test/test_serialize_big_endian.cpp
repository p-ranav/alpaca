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
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
  }
}

TEST_CASE("Serialize uint16_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    uint16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
  }

  // serialize
  {
    my_struct s{12345};
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x30));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x39));
  }
}

TEST_CASE("Serialize positive int8_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 1);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x05));
  }
}

TEST_CASE("Serialize positive int16_t big endian" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x00));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x05));
  }

  // serialize
  {
    my_struct s{12345};
    bytes = serialize<my_struct, options::big_endian>(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x30));
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x39));
  }
}

/// TODO: Enable test once fixed-length encoding is an option
//
// TEST_CASE("Serialize uint32_t big endian" * test_suite("signed_integer")) {
//   struct my_struct {
//     uint32_t value;
//   };

//   std::vector<uint8_t> bytes;

//   // serialize
//   {
//     my_struct s{654321};
//     bytes = serialize<my_struct, options::big_endian>(s);
//     detail::print_bytes(bytes);
//     REQUIRE(bytes.size() == 3);
//     REQUIRE(bytes[0] == static_cast<uint8_t>(0x09));
//     REQUIRE(bytes[1] == static_cast<uint8_t>(0xFB));
//     REQUIRE(bytes[2] == static_cast<uint8_t>(0xF1));
//   }
// }