#include <doctest.hpp>
#include <iostream>
#include <serialize/serialize.h>

using doctest::test_suite;

#define CONSTRUCT_EXPECTED_VALUE(type, value) \
  type expected_value = value; \
  std::vector<uint8_t> expected; \
  std::copy(static_cast<const char *>(static_cast<const void *>(&expected_value)),  \
            static_cast<const char *>(static_cast<const void *>(&expected_value)) + \
                sizeof expected_value, \
            std::back_inserter(expected));

TEST_CASE("Serialize uint8_t" * test_suite("serialize")) {
  struct my_struct {
    uint8_t value;
  };

  my_struct s{5};
  auto bytes = serialize(s);
  REQUIRE(bytes.size() == 2);
  REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint8));
  REQUIRE(bytes[1] == static_cast<uint8_t>(5));
}

TEST_CASE("Serialize uint16_t" * test_suite("serialize")) {
  struct my_struct {
    uint16_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint16_as_uint8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint16));

    CONSTRUCT_EXPECTED_VALUE(uint16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }
}

TEST_CASE("Serialize uint32_t" * test_suite("serialize")) {
  struct my_struct {
    uint32_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint32_as_uint8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint32_as_uint16));

    CONSTRUCT_EXPECTED_VALUE(uint16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint32));

    CONSTRUCT_EXPECTED_VALUE(uint32_t, 12345678);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }
}

TEST_CASE("Serialize uint64_t" * test_suite("serialize")) {
  struct my_struct {
    uint64_t value;
  };

  {
    my_struct s{5};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint64_as_uint8));
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{12345};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 3);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint64_as_uint16));

    CONSTRUCT_EXPECTED_VALUE(uint16_t, 12345);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }

  {
    my_struct s{12345678};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 5);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint64_as_uint32));

    CONSTRUCT_EXPECTED_VALUE(uint32_t, 12345678);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }

  {
    my_struct s{5294967295};
    auto bytes = serialize(s);
    REQUIRE(bytes.size() == 9);
    REQUIRE(bytes[0] == static_cast<uint8_t>(detail::type::uint64));

    CONSTRUCT_EXPECTED_VALUE(uint64_t, 5294967295);
    for (std::size_t i = 1; i < bytes.size() -1; ++i) {
      REQUIRE(bytes[i] == expected[i - 1]);      
    }
  }
}