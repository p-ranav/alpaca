#include <doctest.hpp>
#include <iterator>
#include <serialize/serialize.h>
#include <string>

using doctest::test_suite;

TEST_CASE("Serialize const char*" * test_suite("string")) {
  struct my_struct {
    const char* value;
  };

  my_struct s{"Hello World!"};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 3 + strlen(s.value));
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::string));
  // Size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[2] == static_cast<uint8_t>(strlen(s.value)));
  // Value
  for (std::size_t i = 3; i < bytes.size() -1; ++i) {
    REQUIRE(bytes[i] == static_cast<uint8_t>(s.value[i - 3]));
  }
}

TEST_CASE("Serialize std::string" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  my_struct s{"Hello World!"};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 3 + s.value.size());
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::string));
  // Size
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
  REQUIRE(bytes[2] == static_cast<uint8_t>(s.value.size()));
  // Value
  for (std::size_t i = 3; i < bytes.size() -1; ++i) {
    REQUIRE(bytes[i] == static_cast<uint8_t>(s.value[i - 3]));
  }
}
