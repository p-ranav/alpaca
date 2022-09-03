#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  struct MyStruct {
    std::chrono::milliseconds period;
  };

  MyStruct s{std::chrono::milliseconds{500}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes);

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  // period == 500ms
}