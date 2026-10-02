#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize from a raw pointer (no copy)" *
          test_suite("raw_pointer")) {
  struct Foo {
    int value;
    float other;
  };
  Foo f{5, 3.14f};

  std::vector<uint8_t> bytes;
  auto bytes_written = serialize(f, bytes);

  std::error_code ec;
  const uint8_t *ptr = bytes.data();
  auto foo = deserialize<Foo>(ptr, bytes_written, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(foo.value == 5);
  REQUIRE(foo.other == 3.14f);
}

TEST_CASE("Deserialize from a raw pointer with version and checksum" *
          test_suite("raw_pointer")) {
  // regression test: `bytes.data()` doesn't compile for raw pointers, so
  // with_version/with_checksum previously forced a copy into a vector first
  struct Foo {
    int value;
  };
  Foo f{5};

  uint8_t buf[16];
  uint8_t *write_ptr = buf;
  constexpr auto OPTIONS =
      alpaca::options::with_version | alpaca::options::with_checksum;
  auto bytes_written = serialize<OPTIONS>(f, write_ptr);
  REQUIRE(bytes_written == 9);

  std::error_code ec;
  const uint8_t *read_ptr = buf;
  auto foo = deserialize<OPTIONS, Foo>(read_ptr, bytes_written, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(foo.value == 5);
}

TEST_CASE("Deserialize from a raw pointer with checksum rejects corrupted data" *
          test_suite("raw_pointer")) {
  struct Foo {
    int value;
  };
  Foo f{5};

  uint8_t buf[16];
  uint8_t *write_ptr = buf;
  constexpr auto OPTIONS = alpaca::options::with_checksum;
  auto bytes_written = serialize<OPTIONS>(f, write_ptr);

  // corrupt a data byte
  buf[0] = 0xFF;

  std::error_code ec;
  const uint8_t *read_ptr = buf;
  deserialize<OPTIONS, Foo>(read_ptr, bytes_written, ec);
  REQUIRE((bool)ec == true);
}
