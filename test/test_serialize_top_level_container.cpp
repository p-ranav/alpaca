#include <alpaca/alpaca.h>
#include <doctest.hpp>
#include <map>
#include <optional>
#include <set>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize a top-level std::map with checksum and fixed-length "
          "encoding" *
          test_suite("top_level_container")) {
  // regression test: alpaca::serialize(map, bytes) used to require wrapping
  // the map in a struct; computing the default N via aggregate_arity<T> for
  // a non-aggregate like std::map also triggered runaway template recursion
  constexpr auto OPTIONS =
      alpaca::options::with_checksum | alpaca::options::fixed_length_encoding;
  using serialize_t = std::map<int, bool>;

  serialize_t m{{1, true}, {2, false}, {3, true}};
  std::vector<uint8_t> bytes;
  auto bytes_written = serialize<OPTIONS>(m, bytes);
  REQUIRE(bytes_written > 0);
}

TEST_CASE("Serialize top-level special types with checksum and version" *
          test_suite("top_level_container")) {
  constexpr auto OPTIONS =
      alpaca::options::with_checksum | alpaca::options::with_version;

  {
    std::map<int, bool> m{{1, true}, {2, false}};
    std::vector<uint8_t> bytes;
    REQUIRE(serialize<OPTIONS>(m, bytes) > 0);
  }
  {
    std::vector<int> v{1, 2, 3};
    std::vector<uint8_t> bytes;
    REQUIRE(serialize<OPTIONS>(v, bytes) > 0);
  }
  {
    std::set<int> s{1, 2, 3};
    std::vector<uint8_t> bytes;
    REQUIRE(serialize<OPTIONS>(s, bytes) > 0);
  }
  {
    std::optional<int> o{42};
    std::vector<uint8_t> bytes;
    REQUIRE(serialize<OPTIONS>(o, bytes) > 0);
  }
  {
    std::string s{"hello"};
    std::vector<uint8_t> bytes;
    REQUIRE(serialize<OPTIONS>(s, bytes) > 0);
  }
}

TEST_CASE("Serialize with an explicit N still works for structs where "
          "arity can't be auto-detected (e.g. std::optional fields)" *
          test_suite("top_level_container")) {
  // regression test: routing top-level serialize through a non-aggregate
  // path must not break explicit N, which is required for structs with
  // std::optional fields (aggregate_arity can't auto-detect their arity)
  struct my_struct {
    bool before;
    std::optional<int> value;
    float after;
  };

  my_struct s{true, 5, 3.14f};
  std::vector<uint8_t> bytes;
  serialize<my_struct, 3>(s, bytes);
  REQUIRE(bytes.size() == 7);
}
