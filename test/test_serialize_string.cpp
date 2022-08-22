#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Serialize std::string" * test_suite("string")) {
  struct my_struct {
    std::string value;
  };

  my_struct s{"Hello World!"};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 2 + s.value.size());
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::string));
  REQUIRE(bytes[1] == static_cast<uint8_t>(detail::type::size12));
  // Value
  for (std::size_t i = 2; i < bytes.size() - 1; ++i) {
    REQUIRE(bytes[i] == static_cast<uint8_t>(s.value[i - 2]));
  }
}
