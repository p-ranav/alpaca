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

The above serialization generates a compact vector of 12 bytes:

```
0x61 0x05 0xc3 0xf5 0x48 0x40 0x05 0x48 0x65 0x6c 0x6c 0x6f
^^^^ ^^^^ ^^^^^^^^^^^^^^^^^^^ ^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^
'a'  5    3.14                5    "Hello"
```
