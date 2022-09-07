#include <alpaca/alpaca.h>
#include <filesystem>
using namespace alpaca;

struct GameState {
  int a;
  bool b;
  char c;
  std::string d;
  std::vector<uint64_t> e;
  std::map<std::string, std::array<uint8_t, 3>> f;
};

int main() {

  GameState s{5,
              true,
              'a',
              "Hello World",
              {6, 5, 4, 3, 2, 1},
              {{"abc", {1, 2, 3}}, {"def", {4, 5, 6}}}};

  const auto filename = "savefile.bin";

  {
    // Serialize to file
    std::ofstream os;
    os.open(filename, std::ios::out | std::ios::binary);
    auto bytes_written = serialize(s, os);
    os.close();

    assert(bytes_written == 37);
    assert(std::filesystem::file_size(filename) == 37);
  }

  {
    // Deserialize from file
    auto size = std::filesystem::file_size(filename);
    std::error_code ec;
    std::ifstream is;
    is.open(filename, std::ios::in | std::ios::binary);
    auto recovered = deserialize<GameState>(is, size, ec);
    is.close();

    assert(recovered.a == s.a);
    assert(recovered.b == s.b);
    assert(recovered.c == s.c);
    assert(recovered.d == s.d);
    assert(recovered.e == s.e);
    assert(recovered.f == s.f);
  }

  // delete file
  // std::filesystem::remove(filename);
}
