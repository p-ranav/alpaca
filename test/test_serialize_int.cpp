#include <doctest.hpp>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value)                                  \
  type expected_value = value;                                                 \
  std::vector<uint8_t> expected;                                               \
  std::copy(                                                                   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)),   \
      static_cast<const char *>(static_cast<const void *>(&expected_value)) +  \
          sizeof expected_value,                                               \
      std::back_inserter(expected));

TEST_CASE("Serialize int8_t" * test_suite("signed_integer")) {
  struct my_struct {
    int8_t value;
  };

  my_struct s{5};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 2);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int8));
  REQUIRE(bytes[1] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize int16_t" * test_suite("signed_integer")) {
  struct my_struct {
    int16_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int16_as_int8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int16));

    CONSTRUCT_EXPECTED_VALUE(int16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }
}

TEST_CASE("Serialize int32_t" * test_suite("signed_integer")) {
  struct my_struct {
    int32_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int32_as_int8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int32_as_int16));

    CONSTRUCT_EXPECTED_VALUE(int16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int32));

    CONSTRUCT_EXPECTED_VALUE(int32_t, 12345678);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }
}

TEST_CASE("Serialize int64_t" * test_suite("signed_integer")) {
  struct my_struct {
    int64_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int64_as_int8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int64_as_int16));

    CONSTRUCT_EXPECTED_VALUE(int16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int64_as_int32));

    CONSTRUCT_EXPECTED_VALUE(int32_t, 12345678);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }

  {
    my_struct s{5294967295};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 9);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::int64));

    CONSTRUCT_EXPECTED_VALUE(int64_t, 5294967295);
    for (std::size_t i = 1; i < bytes.size() - 1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);
    }
  }
}