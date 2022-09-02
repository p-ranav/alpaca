#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int with crc32" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  std::array<uint8_t, 5> bytes = {0};

  // serialize
  {
    my_struct s{5};
    serialize<options::with_checksum>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::with_checksum, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize int with crc32 (error)" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  std::array<uint8_t, 20> bytes = {0};
  std::size_t bytes_written = 0;

  // serialize
  {
    my_struct s{5};
    bytes_written = serialize<options::with_checksum>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    bytes[bytes_written - 1] = 0x00; // clear 1 byte
    deserialize<options::with_checksum, my_struct>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::bad_message));
  }
}