#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize into C-array" * test_suite("carray")) {
  uint8_t buf[10];

  struct Foo {
    int value;
  };
  Foo f{5};

  auto bytes_written = alpaca::serialize(f, buf);
  REQUIRE(bytes_written == 1);
  REQUIRE(buf[0] == 0x05);
}

TEST_CASE("Serialize into C-array with version and checksum" *
          test_suite("carray")) {
  uint8_t buf[10];

  struct Foo {
    int value;
  };
  Foo f{5};

  constexpr auto OPTIONS =
      alpaca::options::with_version | alpaca::options::with_checksum;
  auto bytes_written = alpaca::serialize<OPTIONS>(f, buf);
  REQUIRE(bytes_written == 9);
  REQUIRE(buf[4] == 0x05);
}