#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

// The type hash written by options::with_version must depend only on the
// serialized layout (field count and field types), never on sizeof(T),
// alignment or padding, so that bytes written on one platform (say a 32-bit
// microcontroller) verify on another (a 64-bit host).

namespace {

struct plain {
  int32_t a;
  float b;
  std::string c;
};

// same fields, different object size and alignment
struct alignas(64) padded {
  int32_t a;
  float b;
  std::string c;
};

struct nested {
  plain inner;
  std::vector<uint8_t> d;
};

template <typename T> uint32_t version_of() {
  std::vector<uint8_t> typeids;
  std::unordered_map<std::string_view, std::size_t> visited;
  detail::type_info<T>(typeids, visited);
  return crc32_fast(typeids.data(), typeids.size());
}

uint32_t version_in_bytes(const std::vector<uint8_t> &bytes) {
  // the hash is the first 4 bytes, little-endian unless big_endian is set
  uint32_t value = 0;
  std::size_t index = 0, end = bytes.size();
  std::error_code ec;
  detail::from_bytes_crc32<options::none>(value, bytes, index, end, ec);
  return value;
}

} // namespace

TEST_CASE("Type hash does not depend on sizeof / alignment" *
          test_suite("version")) {
  static_assert(sizeof(padded) != sizeof(plain));
  REQUIRE(version_of<plain>() == version_of<padded>());

  std::vector<uint8_t> a, b;
  serialize<options::with_version>(plain{1, 2.0f, "x"}, a);
  serialize<options::with_version>(padded{1, 2.0f, "x"}, b);
  REQUIRE(a == b);

  // and a payload written by one type deserializes as the other
  std::error_code ec;
  auto result = deserialize<options::with_version, padded>(a, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.a == 1);
  REQUIRE(result.c == "x");
}

TEST_CASE("Type hash is the CRC32 of the documented type description" *
          test_suite("version")) {
  using detail::field_type;
  auto code = [](field_type f) { return static_cast<uint8_t>(f); };

  // struct: u16 field count (little-endian), then each field's type
  std::vector<uint8_t> expected{3, 0, code(field_type::int32),
                                code(field_type::float32),
                                code(field_type::string)};
  REQUIRE(version_of<plain>() == crc32_fast(expected.data(), expected.size()));

  // nested struct: its own field count + fields, inline; vector: code + element
  std::vector<uint8_t> expected_nested{2,
                                       0,
                                       3,
                                       0,
                                       code(field_type::int32),
                                       code(field_type::float32),
                                       code(field_type::string),
                                       code(field_type::vector),
                                       code(field_type::uint8)};
  REQUIRE(version_of<nested>() ==
          crc32_fast(expected_nested.data(), expected_nested.size()));

  // the value written in front of the payload is exactly that hash
  std::vector<uint8_t> bytes;
  serialize<options::with_version>(plain{1, 2.0f, "x"}, bytes);
  REQUIRE(version_in_bytes(bytes) == version_of<plain>());
}

TEST_CASE("Type hash still distinguishes layouts" * test_suite("version")) {
  struct a {
    int32_t x;
  };
  struct b {
    int64_t x;
  };
  struct c {
    int32_t x;
    int32_t y;
  };
  REQUIRE(version_of<a>() != version_of<b>());
  REQUIRE(version_of<a>() != version_of<c>());
}
