#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize crc32 error - invalid_argument" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::with_checksum>(s, bytes);
  }

  // deserialize
  {
    auto received = std::vector<uint8_t>(bytes.begin(), bytes.begin() + 2);
    // not enough bytes in input
    // CRC32 check requires at least 4 bytes

    std::error_code ec;
    deserialize<options::with_checksum, my_struct>(received, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::invalid_argument));
  }
}

TEST_CASE("Deserialize crc32 error - bad_message" * test_suite("crc32")) {
  struct my_struct {
    int value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::with_checksum>(s, bytes);
  }

  // deserialize
  {
    auto received = std::vector<uint8_t>{0x05, 0x01, 0x02, 0x03, 0x04};
    //                                        ^^^^^^^^^^^^^^^^^^^^^^ bad crc

    std::error_code ec;
    deserialize<options::with_checksum, my_struct>(received, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::bad_message));
  }
}