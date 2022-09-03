#include <alpaca/alpaca.h>
#include <doctest.hpp>
#include <filesystem>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize complex struct from ifstream" * test_suite("fstream")) {
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

  {
    // Serialize to file
    std::ofstream os;
    os.open("tmp1.bin", std::ios::out | std::ios::binary);
    auto bytes_written = serialize(s, os);
    os.close();
    REQUIRE(bytes_written == 37);
    REQUIRE(std::filesystem::file_size("tmp1.bin") == 37);
  }

  {
    // Deserialize from file
    auto size = std::filesystem::file_size("tmp1.bin");
    std::error_code ec;
    std::ifstream is;
    is.open("tmp1.bin", std::ios::in | std::ios::binary);
    auto recovered = deserialize<my_struct>(is, size, ec);
    is.close();
    REQUIRE(recovered.a == s.a);
    REQUIRE(recovered.b == s.b);
    REQUIRE(recovered.c == s.c);
    REQUIRE(recovered.d == s.d);
    REQUIRE(recovered.e == s.e);
    REQUIRE(recovered.f == s.f);
    std::filesystem::remove("tmp1.bin");
  }
}

TEST_CASE("Deserialize complex struct from ifstream with options" *
          test_suite("fstream")) {
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

  constexpr auto OPTIONS = options::big_endian | options::fixed_length_encoding;
  {
    // Serialize to file
    std::ofstream os;
    os.open("tmp2.bin", std::ios::out | std::ios::binary);
    auto bytes_written = serialize<OPTIONS>(s, os);
    os.close();
    REQUIRE(bytes_written == 117);
    REQUIRE(std::filesystem::file_size("tmp2.bin") == 117);
  }

  {
    // Deserialize from file
    auto size = std::filesystem::file_size("tmp2.bin");
    std::error_code ec;
    std::ifstream is;
    is.open("tmp2.bin", std::ios::in | std::ios::binary);
    auto recovered = deserialize<OPTIONS, my_struct>(is, size, ec);
    is.close();
    REQUIRE(recovered.a == s.a);
    REQUIRE(recovered.b == s.b);
    REQUIRE(recovered.c == s.c);
    REQUIRE(recovered.d == s.d);
    REQUIRE(recovered.e == s.e);
    REQUIRE(recovered.f == s.f);
    std::filesystem::remove("tmp2.bin");
  }
}