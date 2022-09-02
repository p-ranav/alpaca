#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    std::array<int, 3> a;
    std::vector<std::vector<float>> b;
    std::string c;
  };

  MyStruct s{{1, 2, 3}, {{3.14, 1.61}, {2.71, -1}}, {"Hello"}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes); // 28 bytes

  // bytes:
  // {
  //   0x01 0x02 0x03            // array {1, 2, 3}
  //   0x02                      // 2-element vector
  //   0x02                      // 2-element (inner) vector
  //   0xc3 0xf5 0x48 0x40       // vector[0][0] = 3.14
  //   0x7b 0x14 0xce 0x3f       // vector[0][1] = 1.61
  //   0x02                      // 2-element (inner) vector
  //   0xa4 0x70 0x2d 0x40       // vector[1][0] = 2.71
  //   0x00 0x00 0x80 0xbf       // vector[1][1] = -1
  //   0x05                      // start of 5-byte string
  //   0x48 0x65 0x6c 0x6c 0x6f  // string "Hello"
  // }

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert((recovered.a == std::array<int, 3>{1, 2, 3}));
  assert((recovered.b ==
          std::vector<std::vector<float>>{{3.14, 1.61}, {2.71, -1}}));
  assert((recovered.c == std::string{"Hello"}));
}