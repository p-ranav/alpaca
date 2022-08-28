#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Type info for uint8_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint8_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::uint8)
  }));
}

TEST_CASE("Type info for uint16_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint16_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::uint16)
  }));
}

TEST_CASE("Type info for uint32_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint32_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::uint32)
  }));
}

TEST_CASE("Type info for uint64_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint64_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::uint64)
  }));
}

TEST_CASE("Type info for int8_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int8_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::int8)
  }));
}

TEST_CASE("Type info for int16_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int16_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::int16)
  }));
}

TEST_CASE("Type info for int32_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int32_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::int32)
  }));
}

TEST_CASE("Type info for int" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::int32)
  }));
}

TEST_CASE("Type info for float" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<float>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::float32)
  }));
}

TEST_CASE("Type info for double" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<double>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::float64)
  }));
}

TEST_CASE("Type info for char" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<char>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::char_)
  }));
}

TEST_CASE("Type info for bool" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<bool>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::bool_)
  }));
}

TEST_CASE("Type info for enum class" * test_suite("version")) {
  enum class foo { a, b, c };
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<foo>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::enum_class)
  }));
}

TEST_CASE("Type info for string" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<std::string>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
    static_cast<uint8_t>(detail::field_type::string)
  }));
}

TEST_CASE("Type info for array" * test_suite("version")) {

  struct mmy_struct {
    std::array<int, 5> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<mmy_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 7);

  // number of fields in struct
  if (detail::is_system_little_endian()) {
    REQUIRE(typeids[0] == static_cast<uint8_t>(0x01));
    REQUIRE(typeids[1] == static_cast<uint8_t>(0x00));
  }
  else {
    REQUIRE(typeids[0] == static_cast<uint8_t>(0x00));
    REQUIRE(typeids[1] == static_cast<uint8_t>(0x01));
  }

  // size of struct = 20
  if (detail::is_system_little_endian()) {
    REQUIRE(typeids[2] == static_cast<uint8_t>(0x14));
    REQUIRE(typeids[3] == static_cast<uint8_t>(0x00));
  }
  else {
    REQUIRE(typeids[2] == static_cast<uint8_t>(0x00));
    REQUIRE(typeids[3] == static_cast<uint8_t>(0x14));
  }

  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::array));
  REQUIRE(typeids[5] == static_cast<uint8_t>(0x05)); // array size
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::int32)); // array value_type
}