#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize int8_t" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize int16_t (stored as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{99};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 99);
  }
}

TEST_CASE("Deserialize int16_t" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{512};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 512);
  }
}

TEST_CASE("Deserialize int32_t (packed as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize int32_t (packed as int16_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{1600};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 1600);
  }
}

TEST_CASE("Deserialize int32_t" * test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{75535};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 75535);
  }
}

TEST_CASE("Deserialize int64_t (packed as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize int64_t (packed as int16_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{12345};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345);
  }
}

TEST_CASE("Deserialize int64_t (packed as int32_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{12345678};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678);
  }
}

TEST_CASE("Deserialize int64_t" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{5294967295};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5294967295);
  }
}

TEST_CASE("Deserialize int64_t (big positive)" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{12345678910111314};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678910111314);
  }
}

TEST_CASE("Deserialize int64_t (big negative)" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{-12345678910111314};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == -12345678910111314);
  }
}

TEST_CASE("Deserialize signed integer types" * test_suite("integer")) {
  struct my_struct {
    int8_t a;
    int16_t b;
    int32_t c;
    int64_t d;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{-5, -12345, -12345678, -5294967295};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.a == -5);
    REQUIRE(result.b == -12345);
    REQUIRE(result.c == -12345678);
    REQUIRE(result.d == -5294967295);
  }
}

TEST_CASE("Deserialize signed and unsigned integer types" *
          test_suite("integer")) {
  struct my_struct {
    int8_t a;
    int16_t b;
    int32_t c;
    int64_t d;
    uint8_t e;
    uint16_t f;
    uint32_t g;
    uint64_t h;
  };

  std::array<uint8_t, 100> bytes;

  // serialize
  {
    my_struct s{-5, -12345, -12345678, -5294967295,
                5,  12345,  12345678,  5294967295};
    serialize(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.a == -5);
    REQUIRE(result.b == -12345);
    REQUIRE(result.c == -12345678);
    REQUIRE(result.d == -5294967295);
    REQUIRE(result.e == 5);
    REQUIRE(result.f == 12345);
    REQUIRE(result.g == 12345678);
    REQUIRE(result.h == 5294967295);
  }
}