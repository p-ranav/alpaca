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

TEST_CASE("Deserialize big-endian uint8_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 1);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian uint8_t - error message_size" *
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

TEST_CASE("Deserialize big-endian uint16_t (stored as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{99};
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 99);
  }
}

TEST_CASE("Deserialize big-endian uint16_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{512};
    serialize<options::big_endian>(s, bytes);
    REQUIRE(bytes.size() == 2);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 512);
  }
}

TEST_CASE("Deserialize big-endian uint32_t (packed as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 4); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 1); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian uint32_t (packed as uint16_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{1600};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 4); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 2); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 1600);
  }
}

TEST_CASE("Deserialize big-endian uint32_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{75535};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 4); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 3); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 75535);
  }
}

TEST_CASE("Deserialize big-endian uint64_t (packed as uint8_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 8); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 1); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian uint64_t (packed as uint16_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 8); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 2); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345);
  }
}

TEST_CASE("Deserialize big-endian uint64_t (packed as uint32_t)" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345678};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 8); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 4); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678);
  }
}

TEST_CASE("Deserialize big-endian uint64_t" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5294967295};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() == 8); // fixed-width encoding is used
    } else {
      REQUIRE(bytes.size() == 5); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5294967295);
  }
}

TEST_CASE("Deserialize big-endian unsigned integer types" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
    uint64_t d;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5, 12345, 12345678, 5294967295};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.a == 5);
    REQUIRE(result.b == 12345);
    REQUIRE(result.c == 12345678);
    REQUIRE(result.d == 5294967295);
  }
}

TEST_CASE("Deserialize big-endian unsigned integer types" *
          test_suite("unsigned_integer")) {
  struct my_struct {
    uint8_t e;
    uint16_t f;
    uint32_t g;
    uint64_t h;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5, 12345, 12345678, 5294967295};
    serialize<options::big_endian>(s, bytes);
    if constexpr (detail::is_system_little_endian()) {
      REQUIRE(bytes.size() ==
              15); // fixed-width encoding is used for uint32 and uint64 types
    } else {
      REQUIRE(bytes.size() == 12); // VLQ
    }
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.e == 5);
    REQUIRE(result.f == 12345);
    REQUIRE(result.g == 12345678);
    REQUIRE(result.h == 5294967295);
  }
}

TEST_CASE("Deserialize big-endian int8_t" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian int16_t (stored as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{99};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 99);
  }
}

TEST_CASE("Deserialize big-endian int16_t" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{512};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 512);
  }
}

TEST_CASE("Deserialize big-endian int32_t (packed as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian int32_t (packed as int16_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{1600};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 1600);
  }
}

TEST_CASE("Deserialize big-endian int32_t" * test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{75535};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 75535);
  }
}

TEST_CASE("Deserialize big-endian int64_t (packed as int8_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5);
  }
}

TEST_CASE("Deserialize big-endian int64_t (packed as int16_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345);
  }
}

TEST_CASE("Deserialize big-endian int64_t (packed as int32_t)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345678};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678);
  }
}

TEST_CASE("Deserialize big-endian int64_t" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5294967295};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5294967295);
  }
}

TEST_CASE("Deserialize big-endian int64_t (big positive)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345678910111314};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 12345678910111314);
  }
}

TEST_CASE("Deserialize big-endian int64_t (big negative)" *
          test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{-12345678910111314};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == -12345678910111314);
  }
}

TEST_CASE("Deserialize big-endian signed integer types" *
          test_suite("integer")) {
  struct my_struct {
    int8_t a;
    int16_t b;
    int32_t c;
    int64_t d;
  };

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{-5, -12345, -12345678, -5294967295};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.a == -5);
    REQUIRE(result.b == -12345);
    REQUIRE(result.c == -12345678);
    REQUIRE(result.d == -5294967295);
  }
}

TEST_CASE("Deserialize big-endian signed and unsigned integer types" *
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{-5, -12345, -12345678, -5294967295,
                5,  12345,  12345678,  5294967295};
    serialize<options::big_endian>(s, bytes);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<options::big_endian, my_struct>(bytes, ec);
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