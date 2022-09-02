#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int into struct { int }" * test_suite("version")) {
  struct my_struct {
    int value;
  };

  std::array<uint8_t, 20> bytes;

  {
    my_struct s{5};
    serialize<options::with_version>(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<options::with_version, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize vector<int> into struct { vector<int> }" *
          test_suite("version")) {

  std::array<uint8_t, 20> bytes;

  {
    struct my_struct {
      std::vector<int> value;
    };
    my_struct s{{1, 2, 3, 4, 5}};
    serialize<options::with_version>(s, bytes);
  }

  {
    struct my_struct {
      std::vector<int> value;
    };
    std::error_code ec;
    auto result = deserialize<options::with_version, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE((result.value == std::vector<int>{1, 2, 3, 4, 5}));
  }
}