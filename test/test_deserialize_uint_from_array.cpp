#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Deserialize uint8_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{5};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 1);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
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

TEST_CASE("Deserialize uint16_t (stored as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{99};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 99);
  }
}

TEST_CASE("Deserialize uint16_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{512};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 512);
  }
}

TEST_CASE("Deserialize uint32_t (packed as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{5};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 1);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize uint32_t (packed as uint16_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{1600};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 1600);
  }
}

TEST_CASE("Deserialize uint32_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{75535};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 3);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 75535);
  }
}

TEST_CASE("Deserialize uint64_t (packed as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{5};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 1);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize uint64_t (packed as uint16_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{12345};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345);
  }
}

TEST_CASE("Deserialize uint64_t (packed as uint32_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{12345678};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 4);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678);
  }
}

TEST_CASE("Deserialize uint64_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::array<uint8_t, 10> bytes;

  // serialize
  {
    my_struct s{5294967295};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 5);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5294967295);
  }
}

TEST_CASE("Deserialize unsigned integer types" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  std::array<uint8_t, 32> bytes;

  // serialize
  {
    my_struct s{5, 12345, 12345678, 5294967295};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.a == 5);
    REQUIRE(result.b == 12345);
    REQUIRE(result.c == 12345678);
    REQUIRE(result.d == 5294967295);
  }
}

TEST_CASE("Deserialize unsigned integer types" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t e;
    uint16_t f;
    uint32_t g;
    uint64_t h;
  };

  std::array<uint8_t, 32> bytes;

  // serialize
  {
    my_struct s{5, 12345, 12345678, 5294967295};
    auto bytes_written = serialize(s, bytes);
    REQUIRE(bytes_written == 12);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.e == 5);
    REQUIRE(result.f == 12345);
    REQUIRE(result.g == 12345678);
    REQUIRE(result.h == 5294967295);
  }
}