#include <alpaca/alpaca.h>
#include <cstdint>
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 1);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{99};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{512};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 1);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{1600};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{75535};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 3);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 1);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{12345678};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 4);
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5294967295};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 5);
  }

  // deserialize
  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.value == 5294967295);
  }
}

TEST_CASE("Deserialize uint32_t UINT32_MAX" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint32_t value;
  };

  // regression test: a 32-bit value needing the maximum number of varint
  // bytes (5), previously the decoder stopped reading after 4 bytes
  std::vector<uint8_t> bytes;
  my_struct s{UINT32_MAX};
  serialize(s, bytes);
  REQUIRE(bytes.size() == 5);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.value == UINT32_MAX);
}

TEST_CASE("Deserialize uint64_t UINT64_MAX" * test_suite("unsigned_integer")) {
  struct my_struct {
    uint64_t value;
  };

  // regression test: a 64-bit value needing the maximum number of varint
  // bytes (10), previously the decoder stopped reading after 8 bytes
  std::vector<uint8_t> bytes;
  my_struct s{UINT64_MAX};
  serialize(s, bytes);
  REQUIRE(bytes.size() == 10);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.value == UINT64_MAX);
}

TEST_CASE("Deserialize uint64_t UINT64_MAX followed by other fields" *
          test_suite("unsigned_integer")) {
  // regression test: when decoding a UINT64_MAX field stopped short, the
  // read cursor desynced and corrupted every field that followed it
  struct my_struct {
    char a;
    int b;
    uint64_t c;
    float d;
    bool e;
  };

  my_struct s{'a', 5, UINT64_MAX, 3.14f, true};
  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.a == 'a');
  REQUIRE(result.b == 5);
  REQUIRE(result.c == UINT64_MAX);
  REQUIRE(result.d == doctest::Approx(3.14f));
  REQUIRE(result.e == true);
}

TEST_CASE("Deserialize nested struct with a max-length varint followed by a "
          "string" *
          test_suite("unsigned_integer")) {
  // regression test (issue #16): a desynced cursor after a short varint
  // decode corrupted the following std::string's length, surfacing as a
  // "value too large" deserialize error
  struct my_struct_inner {
    std::string filed1{};
    unsigned long long filed2{};
    unsigned long long filed3{};
    unsigned int filed4{};
    unsigned int filed5{};
    unsigned long long filed6{};
  };

  struct my_struct {
    unsigned int filed1{};
    std::string filed2{};
    unsigned long long filed3{};
    unsigned long long filed4{};
    unsigned long long filed5{};
    unsigned long long filed6{};
    my_struct_inner filed7{};
  };

  my_struct s{};
  s.filed7.filed1 = std::string(128, 'x');
  s.filed7.filed2 = 0x91e7b3ed1632245c; // needs the maximum varint length
  s.filed7.filed3 = 2;
  s.filed7.filed5 = 3;
  s.filed7.filed6 = 5;

  s.filed2 = std::string(624, 'y');
  s.filed1 = 1;
  s.filed3 = 0x91e7b3ed1632245c; // needs the maximum varint length
  s.filed4 = 3;
  s.filed6 = 4;
  s.filed7.filed4 = 2;

  std::vector<uint8_t> bytes;
  serialize(s, bytes);

  std::error_code ec;
  auto result = deserialize<my_struct>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result.filed1 == s.filed1);
  REQUIRE(result.filed2 == s.filed2);
  REQUIRE(result.filed3 == s.filed3);
  REQUIRE(result.filed4 == s.filed4);
  REQUIRE(result.filed6 == s.filed6);
  REQUIRE(result.filed7.filed1 == s.filed7.filed1);
  REQUIRE(result.filed7.filed2 == s.filed7.filed2);
  REQUIRE(result.filed7.filed3 == s.filed7.filed3);
  REQUIRE(result.filed7.filed5 == s.filed7.filed5);
  REQUIRE(result.filed7.filed6 == s.filed7.filed6);
  REQUIRE(result.filed7.filed4 == s.filed7.filed4);
}

TEST_CASE("Deserialize unsigned integer types" *
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

  std::vector<uint8_t> bytes;

  // serialize
  {
    my_struct s{5, 12345, 12345678, 5294967295};
    serialize(s, bytes);
    REQUIRE(bytes.size() == 12);
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