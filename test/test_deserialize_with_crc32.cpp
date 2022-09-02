#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int with crc32" * test_suite("crc32")) {
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::with_checksum>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    bytes.pop_back(); // remove 1 byte
    deserialize<options::with_checksum, my_struct>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::bad_message));
  }
}