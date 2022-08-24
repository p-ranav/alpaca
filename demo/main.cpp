#include <cassert>
#include <structbyte/structbyte.h>
using namespace structbyte;

struct MyStruct {
  char a;
  int b;
  uint64_t c;  
  float d;
  bool e;
};

int main() {

  MyStruct s{'a', 5, 12345, 3.14f, true};

  // Serialize
  auto bytes = serialize(s); // 10 bytes
  std::cout << bytes.size();

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
  assert(recovered.a == 'a');
  assert(recovered.b == 5);
  assert(recovered.c == 12345);
  assert(recovered.d == 3.14f);
  assert(recovered.e == true);
}
