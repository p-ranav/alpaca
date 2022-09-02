#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    std::optional<int> a;
    std::optional<float> b;
    std::optional<std::string> c;
    std::optional<std::vector<bool>> d;
  };

  MyStruct s{5, 3.14f, std::nullopt,
             std::vector<bool>{true, false, true, false}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize<MyStruct, 4>(
      s, bytes); // 14 bytes
                 // ^^^^^^^^^^^
                 //    specify the number of fields (4) in struct manually
                 //    alpaca fails at correctly detecting this due to the
                 //    nature of std::optional

  // bytes:
  // {
  //   0x01                    // optional has_value = true
  //   0x05                    // value = 5
  //   0x01                    // optional has_value = true
  //   0xc3 0xf5 0x48 0x40     // value = 3.14f
  //   0x00                    // optional has_value = false
  //   0x01                    // optional has_value = true
  //   0x04                    // 4-element vector
  //   0x01 0x00 0x01 0x00     // {true, false, true, false}
  // }

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert(recovered.a.value() == 5);
  assert(recovered.b.value() == 3.14f);
  assert(recovered.c.has_value() == false);
  assert((recovered.d.value() == std::vector<bool>{true, false, true, false}));
}