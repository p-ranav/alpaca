#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t>> a;
    std::set<int> b;
  };

  MyStruct s{{{"red", std::make_tuple(255, 0, 0)},
              {"green", std::make_tuple(0, 255, 0)},
              {"blue", std::make_tuple(0, 0, 255)}},
             {1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes); // 30 bytes

  // bytes:
  // {
  //   0x03                      // 3-element map
  //   0x04                      // start of 4-byte string
  //   0x62 0x6c 0x75 0x65       // string "blue"
  //   0x00 0x00 0xff            // tuple {0, 0, 255}
  //   0x05                      // start of 5-byte string
  //   0x67 0x72 0x65 0x65 0x6e  // string "green"
  //   0x00 0xff 0x00            // tuple {0, 255, 0}
  //   0x03                      // 3-byte string
  //   0x72 0x65 0x64            // string "red"
  //   0xff 0x00 0x00            // tuple {255, 0, 0}
  //   0x04                      // 4-element set
  //   0x01 0x02 0x03 0x04       // set {1, 2, 3, 4}
  // }

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert((recovered.a ==
          std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t>>{
              {"red", std::make_tuple(255, 0, 0)},
              {"green", std::make_tuple(0, 255, 0)},
              {"blue", std::make_tuple(0, 0, 255)}}));
  assert((recovered.b == std::set<int>{1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}));
}