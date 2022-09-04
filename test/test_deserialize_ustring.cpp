#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize std::u16string" * test_suite("string")) {
  struct my_struct {
    std::wstring name;
    std::u16string example;
    std::u32string greeting;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{L"緋村 剣心", u"This is a string", U"Hello, 世界"};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.name == L"緋村 剣心");
    REQUIRE(result.example == u"This is a string");
    REQUIRE(result.greeting == U"Hello, 世界");
  }
}