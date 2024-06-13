#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

constexpr auto OPTIONS = options::fixed_length_encoding | options::force_aligned_access;

TEST_SUITE("unaligned access") {

  TEST_CASE("Complex structure with non-aligned fields") {

    struct Complex {
      uint8_t byte1{0};      // 1 byte
      uint32_t field1{0};    // 4 bytes, starts at offset 1
      uint8_t byte2{0};      // 1 byte, starts at offset 5
      uint64_t field2{0};    // 8 bytes, starts at offset 6
      uint32_t field3{0};    // 4 bytes, starts at offset 14
      uint8_t byte3{0};      // 1 byte, starts at offset 18
    };

    Complex data{0x01, 0x12345678, 0x02, 0x1234567890ABCDEF, 0xDEADBEEF, 0x03};

    std::vector<uint8_t> bytes;
    alpaca::serialize<OPTIONS>(data, bytes);

    std::error_code err;
    auto reconstructed = alpaca::deserialize<OPTIONS, Complex>(bytes, err);

    REQUIRE(!err);

    REQUIRE(reconstructed.byte1 == data.byte1);
    REQUIRE(reconstructed.field1 == data.field1);
    REQUIRE(reconstructed.byte2 == data.byte2);
    REQUIRE(reconstructed.field2 == data.field2);
    REQUIRE(reconstructed.field3 == data.field3);
    REQUIRE(reconstructed.byte3 == data.byte3);
  }

  TEST_CASE("Nested structures with non-aligned fields") {

    struct Inner {
      uint8_t byte1{0};
      uint64_t field1{0};
    };

    struct Outer {
      uint8_t byte2{0};
      uint32_t field2{0};
      Inner inner;
      uint8_t byte3{0};
      uint64_t field3{0};
    };

    Outer data{0x04, 0x56789ABC, {0x05, 0x1234567890ABCDEF}, 0x06, 0xFEDCBA9876543210};

    std::vector<uint8_t> bytes;
    alpaca::serialize<OPTIONS>(data, bytes);

    std::error_code err;
    auto reconstructed = alpaca::deserialize<OPTIONS, Outer>(bytes, err);

    REQUIRE(!err);

    REQUIRE(reconstructed.byte2 == data.byte2);
    REQUIRE(reconstructed.field2 == data.field2);
    REQUIRE(reconstructed.inner.byte1 == data.inner.byte1);
    REQUIRE(reconstructed.inner.field1 == data.inner.field1);
    REQUIRE(reconstructed.byte3 == data.byte3);
    REQUIRE(reconstructed.field3 == data.field3);
  }

}