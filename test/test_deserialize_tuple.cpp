#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

TEST_CASE("Deserialize tuple<int, float, bool>" * test_suite("tuple")) {
  struct my_struct {
    std::tuple<int, float, bool, std::string, char> values;
  };

  std::vector<uint8_t> bytes;
  {
    my_struct s{std::make_tuple(5, 3.14, true, "Hello", 'i')};
    bytes = serialize(s);
    detail::print_bytes(bytes);
  }
  {
    auto result = deserialize<my_struct>(bytes);
    REQUIRE(std::get<0>(result.values) == 5);
    REQUIRE(std::get<1>(result.values) == 3.14f);
    REQUIRE(std::get<2>(result.values) == true);
    REQUIRE(std::get<3>(result.values) == std::string{"Hello"});
    REQUIRE(std::get<4>(result.values) == 'i');
  }
}