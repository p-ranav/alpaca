#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize uint8_t into int8_t" * test_suite("signed_integer")) {

  std::vector<uint8_t> bytes;

  // serialize
  {
    struct my_struct {
      uint8_t value;
    };
    my_struct s{5};
    bytes = serialize(s);
  }

  // deserialize
  {
    struct my_new_struct {
      int8_t value;
    };

    std::error_code ec;
    auto result = deserialize<my_new_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize uint8_t - error message_size" *
          test_suite("signed_integer")) {

  std::vector<uint8_t> bytes{};

  // deserialize
  {
    struct my_new_struct {
      uint8_t value;
    };

    std::error_code ec;
    deserialize<my_new_struct>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() ==
            static_cast<int>(
                std::errc::message_size)); // 1 byte expected for uint8_t
  }
}

TEST_CASE("Deserialize uint8_t into int16_t - error message_size" *
          test_suite("signed_integer")) {

  std::vector<uint8_t> bytes;

  // serialize
  {
    struct my_struct {
      uint8_t value;
    };
    my_struct s{5};
    bytes = serialize(s);
  }

  // deserialize
  {
    struct my_new_struct {
      int16_t value;
    };

    std::error_code ec;
    deserialize<my_new_struct>(bytes, ec);
    REQUIRE((bool)ec == true);
    REQUIRE(ec.value() ==
            static_cast<int>(
                std::errc::message_size)); // 2 bytes expected for uint16_t
  }
}

TEST_CASE("Deserialize uint8_t into int32_t" * test_suite("signed_integer")) {

  std::vector<uint8_t> bytes;

  // serialize
  {
    struct my_struct {
      uint8_t value;
    };
    my_struct s{5};
    bytes = serialize(s);
  }

  // deserialize
  {
    struct my_new_struct {
      int value;
    };

    std::error_code ec;
    auto result = deserialize<my_new_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize uint8_t into int64_t" * test_suite("signed_integer")) {

  std::vector<uint8_t> bytes;

  // serialize
  {
    struct my_struct {
      uint8_t value;
    };
    my_struct s{5};
    bytes = serialize(s);
  }

  // deserialize
  {
    struct my_new_struct {
      int64_t value;
    };

    std::error_code ec;
    auto result = deserialize<my_new_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}