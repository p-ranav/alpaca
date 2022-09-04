#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  struct MyStruct {
    std::time_t timestamp;
  };

  auto timestamp =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  MyStruct s{timestamp};

  constexpr auto OPTIONS = options::big_endian | options::fixed_length_encoding;

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize<OPTIONS>(s, bytes);

  // bytes: {0x00 0x00 0x00 0x00 0x63 0x13 0xeb 0x21}

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<OPTIONS, MyStruct>(bytes, ec);

  // timestamp: 1662249761
  //
  // Human time:
  // GMT: Sunday, September 4, 2022 12:02:41 AM
}