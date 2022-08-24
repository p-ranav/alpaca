#include <cassert>
#include <serialize/serialize.h>

struct MyStruct {
  char a;
  int b;
  float c;
  std::string d;
  uint64_t e;
  bool f;
};

int main() {

  MyStruct s{'a', 5, 3.14f, "Hello", 12345, true};

  // Serialize
  auto bytes = serialize(s); // 15 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}