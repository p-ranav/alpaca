#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Backwards Compatibility (new field added in struct)" *
          test_suite("backwards_compatibility")) {

  std::vector<uint8_t> bytes;
  {
    // client side is updated to use
    // a new structure
    struct my_struct {
      int old_field_1;
      float old_field_2;
      std::string new_field_1;
    };

    my_struct s{5, 3.14f, "Hello"};
    serialize(s, bytes);
  }

  {
    // server side is still compiled to
    // deserialize the older version of
    // the struct
    struct my_struct {
      int old_field_1;
      float old_field_2;
    };
    std::error_code ec;
    auto s = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(s.old_field_1 == 5);
    REQUIRE(s.old_field_2 == 3.14f);
  }
}