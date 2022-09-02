#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    char a;
    int b;
    uint64_t c;
    float d;
    bool e;
  };

  MyStruct s{'a', 5, 12345, 3.14f, true};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes); // 9 bytes

  // bytes:
  // {
  //   0x61                  // char 'a'
  //   0x05                  // int 5
  //   0xb9 0x60             // uint 12345
  //   0xc3 0xf5 0x48 0x40   // float 3.14f
  //   0x01                  // bool true
  // }

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert(recovered.a == 'a');
  assert(recovered.b == 5);
  assert(recovered.c == 12345);
  assert(recovered.d == 3.14f);
  assert(recovered.e == true);
}