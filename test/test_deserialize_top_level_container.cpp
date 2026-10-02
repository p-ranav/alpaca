#include <alpaca/alpaca.h>
#include <doctest.hpp>
#include <map>
#include <optional>
#include <set>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize a top-level std::map with checksum and fixed-length "
          "encoding" *
          test_suite("top_level_container")) {
  constexpr auto OPTIONS =
      alpaca::options::with_checksum | alpaca::options::fixed_length_encoding;
  using serialize_t = std::map<int, bool>;

  serialize_t m{{1, true}, {2, false}, {3, true}};
  std::vector<uint8_t> bytes;
  serialize<OPTIONS>(m, bytes);

  std::error_code ec;
  auto result = deserialize<OPTIONS, serialize_t>(bytes, ec);
  REQUIRE((bool)ec == false);
  REQUIRE(result == m);
}

TEST_CASE("Deserialize top-level special types with checksum and version" *
          test_suite("top_level_container")) {
  constexpr auto OPTIONS =
      alpaca::options::with_checksum | alpaca::options::with_version;

  {
    std::map<int, bool> m{{1, true}, {2, false}};
    std::vector<uint8_t> bytes;
    serialize<OPTIONS>(m, bytes);
    std::error_code ec;
    auto result = deserialize<OPTIONS, std::map<int, bool>>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result == m);
  }
  {
    std::vector<int> v{1, 2, 3};
    std::vector<uint8_t> bytes;
    serialize<OPTIONS>(v, bytes);
    std::error_code ec;
    auto result = deserialize<OPTIONS, std::vector<int>>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result == v);
  }
  {
    std::set<int> s{1, 2, 3};
    std::vector<uint8_t> bytes;
    serialize<OPTIONS>(s, bytes);
    std::error_code ec;
    auto result = deserialize<OPTIONS, std::set<int>>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result == s);
  }
  {
    std::optional<int> o{42};
    std::vector<uint8_t> bytes;
    serialize<OPTIONS>(o, bytes);
    std::error_code ec;
    auto result = deserialize<OPTIONS, std::optional<int>>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result == o);
  }
  {
    std::string s{"hello"};
    std::vector<uint8_t> bytes;
    serialize<OPTIONS>(s, bytes);
    std::error_code ec;
    auto result = deserialize<OPTIONS, std::string>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result == s);
  }
}

TEST_CASE("Deserialize with checksum rejects a corrupted top-level map" *
          test_suite("top_level_container")) {
  constexpr auto OPTIONS = alpaca::options::with_checksum;
  using serialize_t = std::map<int, bool>;

  serialize_t m{{1, true}, {2, false}};
  std::vector<uint8_t> bytes;
  serialize<OPTIONS>(m, bytes);

  // corrupt a data byte
  bytes[0] ^= 0xFF;

  std::error_code ec;
  deserialize<OPTIONS, serialize_t>(bytes, ec);
  REQUIRE((bool)ec == true);
}
