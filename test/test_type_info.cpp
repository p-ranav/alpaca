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
                          static_cast<uint8_t>(detail::field_type::uint8)}));
}

TEST_CASE("Type info for uint16_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint16_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::uint16)}));
}

TEST_CASE("Type info for uint32_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint32_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::uint32)}));
}

TEST_CASE("Type info for uint64_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<uint64_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::uint64)}));
}

TEST_CASE("Type info for int8_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int8_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::int8)}));
}

TEST_CASE("Type info for int16_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int16_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::int16)}));
}

TEST_CASE("Type info for int32_t" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int32_t>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::int32)}));
}

TEST_CASE("Type info for int" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<int>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::int32)}));
}

TEST_CASE("Type info for float" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<float>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::float32)}));
}

TEST_CASE("Type info for double" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<double>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::float64)}));
}

TEST_CASE("Type info for char" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<char>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::char_)}));
}

TEST_CASE("Type info for bool" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<bool>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::bool_)}));
}

TEST_CASE("Type info for enum class" * test_suite("version")) {
  enum class foo { a, b, c };
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<foo>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{static_cast<uint8_t>(
                          detail::field_type::enum_class)}));
}

TEST_CASE("Type info for string" * test_suite("version")) {
  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<std::string>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 1);
  REQUIRE((typeids == std::vector<uint8_t>{
                          static_cast<uint8_t>(detail::field_type::string)}));
}

TEST_CASE("Type info for array" * test_suite("version")) {

  struct my_struct {
    std::array<int, 5> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 7);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::array));
  REQUIRE(typeids[5] == static_cast<uint8_t>(0x05)); // array size
  REQUIRE(typeids[6] ==
          static_cast<uint8_t>(detail::field_type::int32)); // array value_type
}

TEST_CASE("Type info for map" * test_suite("version")) {

  struct my_struct {
    std::map<int, std::string> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 7);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::map));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::string));
}

TEST_CASE("Type info for unordered_map" * test_suite("version")) {

  struct my_struct {
    std::unordered_map<char, std::string> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 7);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] ==
          static_cast<uint8_t>(detail::field_type::unordered_map));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::char_));
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::string));
}

TEST_CASE("Type info for optional" * test_suite("version")) {

  struct my_struct {
    std::optional<float> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct, 1>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::optional));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::float32));
}

TEST_CASE("Type info for pair" * test_suite("version")) {

  struct my_struct {
    std::pair<int, double> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 7);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::pair));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::float64));
}

TEST_CASE("Type info for set" * test_suite("version")) {

  struct my_struct {
    std::set<std::string> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::set));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::string));
}

TEST_CASE("Type info for unordered_set" * test_suite("version")) {

  struct my_struct {
    std::unordered_set<std::string> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] ==
          static_cast<uint8_t>(detail::field_type::unordered_set));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::string));
}

TEST_CASE("Type info for string" * test_suite("version")) {

  struct my_struct {
    std::string value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 5);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::string));
}

TEST_CASE("Type info for tuple" * test_suite("version")) {

  struct my_struct {
    std::tuple<int, float, char> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 8);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::tuple));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::float32));
  REQUIRE(typeids[7] == static_cast<uint8_t>(detail::field_type::char_));
}

#if !defined(_MSC_VER) && !defined(__MINGW32__)

TEST_CASE("Type info for unique_ptr" * test_suite("version")) {

  struct my_struct {
    std::unique_ptr<int> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::unique_ptr));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
}

TEST_CASE("Type info for unique_ptr (recursive)" * test_suite("version")) {

  struct my_struct {
    std::unique_ptr<my_struct> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::unique_ptr));
  REQUIRE(typeids[5] == static_cast<uint8_t>(0x01));
  REQUIRE(struct_visitor_map.size() == 1);
  auto it = struct_visitor_map.begin();
  REQUIRE(it->second == 0x01);
}

#endif

TEST_CASE("Type info for variant" * test_suite("version")) {

  struct my_struct {
    std::variant<int, float, char> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 8);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::variant));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
  REQUIRE(typeids[6] == static_cast<uint8_t>(detail::field_type::float32));
  REQUIRE(typeids[7] == static_cast<uint8_t>(detail::field_type::char_));
}

TEST_CASE("Type info for vector" * test_suite("version")) {

  struct my_struct {
    std::vector<int> value;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 6);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::vector));
  REQUIRE(typeids[5] == static_cast<uint8_t>(detail::field_type::int32));
}

TEST_CASE("Type info for nested_struct" * test_suite("version")) {

  struct my_struct {
    int value;
    struct nested_struct {
      float value;
    };
    nested_struct nested;
  };

  std::vector<uint8_t> typeids{};
  std::unordered_map<std::string_view, size_t> struct_visitor_map{};
  detail::type_info<my_struct>(typeids, struct_visitor_map);
  REQUIRE(typeids.size() == 10);
  // ignoring these:
  //   2 bytes for num fields in struct
  //   2 bytes for sizeof(struct)
  REQUIRE(typeids[4] == static_cast<uint8_t>(detail::field_type::int32));
  // 4 bytes of info for nested struct (num fields + sizeof)
  REQUIRE(typeids[9] == static_cast<uint8_t>(detail::field_type::float32));
}