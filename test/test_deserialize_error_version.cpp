#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int into another struct { uint16_t }" *
          test_suite("version")) {

  std::vector<uint8_t> bytes;

  {
    struct my_struct {
      int value;
    };
    my_struct s{5};
    bytes =
        serialize<my_struct, std::vector<uint8_t>, options::with_version>(s);
  }

  {
    struct my_struct {
      uint16_t value;
    };
    std::error_code ec;
    deserialize<my_struct, options::with_version>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::invalid_argument));
  }
}

TEST_CASE("Deserialize int into struct { int,int,int }" *
          test_suite("version")) {

  std::vector<uint8_t> bytes;

  {
    struct my_struct {
      int value;
    };
    my_struct s{5};
    bytes =
        serialize<my_struct, std::vector<uint8_t>, options::with_version>(s);
  }

  {
    struct my_struct {
      int a;
      int b;
      int c;
    };
    std::error_code ec;
    deserialize<my_struct, options::with_version>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::invalid_argument));
  }
}

TEST_CASE("Deserialize vector<int> into struct { vector<float> }" *
          test_suite("version")) {

  std::vector<uint8_t> bytes;

  {
    struct my_struct {
      std::vector<int> value;
    };
    my_struct s{{1, 2, 3, 4, 5}};
    bytes =
        serialize<my_struct, std::vector<uint8_t>, options::with_version>(s);
  }

  {
    struct my_struct {
      std::vector<float> value;
    };
    std::error_code ec;
    deserialize<my_struct, options::with_version>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() == static_cast<int>(std::errc::invalid_argument));
  }
}