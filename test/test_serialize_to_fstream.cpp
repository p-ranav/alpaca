#include <alpaca/alpaca.h>
#include <doctest.hpp>
#include <filesystem>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize complex struct to fstream" * test_suite("fstream")) {
  struct my_struct {
    int a;
    bool b;
    char c;
    std::string d;
    std::vector<uint64_t> e;
    std::map<std::string, std::array<uint8_t, 3>> f;
  };

  my_struct s{5,
              true,
              'a',
              "Hello World",
              {6, 5, 4, 3, 2, 1},
              {{"abc", {1, 2, 3}}, {"def", {4, 5, 6}}}};

  // Serialize to file
  std::ofstream os;
  os.open("tmp.bin", std::ios::out | std::ios::binary);
  auto bytes_written = serialize(s, os);
  os.close();
  REQUIRE(bytes_written == 37);
  REQUIRE(std::filesystem::file_size("tmp.bin") == 37);
  std::filesystem::remove("tmp.bin");
}

TEST_CASE("Serialize complex struct to fstream with options" * test_suite("fstream")) {
  struct my_struct {
    int a;
    bool b;
    char c;
    std::string d;
    std::vector<uint64_t> e;
    std::map<std::string, std::array<uint8_t, 3>> f;
  };

  my_struct s{5,
              true,
              'a',
              "Hello World",
              {6, 5, 4, 3, 2, 1},
              {{"abc", {1, 2, 3}}, {"def", {4, 5, 6}}}};

  constexpr auto OPTIONS = options::big_endian |
                           options::fixed_length_encoding |
                           options::with_version;

  // Serialize to file
  std::ofstream os;
  os.open("tmp.bin", std::ios::out | std::ios::binary);
  auto bytes_written = serialize<OPTIONS>(s, os);
  os.close();
  REQUIRE(bytes_written == 121);
  REQUIRE(std::filesystem::file_size("tmp.bin") == 121);
  std::filesystem::remove("tmp.bin");
}