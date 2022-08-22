# serialize
Serialization library in modern C++ - serialize a struct to bytes and back without any macros or boilerplate.

```cpp
#include <cassert>
#include <serialize/serialize.h>

struct Message {
  char a;
  int b;
  float c;
  std::string d;
};

int main() {

  Message msg{'a', 5, 3.14f, "Hello"};

  // Serialize
  auto bytes = serialize(s); // 12 bytes
  
  // Deserialize
  auto recovered = deserialize<Message>(bytes);
  assert(recovered.a == 'a');
  assert(recovered.b == 5);
  assert(recovered.c == 3.14f);
  assert(recovered.d == "Hello");  
}
```
