# serialize
Serialization library in modern C++ - serialize a struct to bytes and back without any macros or boilerplate.

```cpp
#include <cassert>
#include <serialize/serialize.h>

int main() {

  struct Foo {
    char a;
    int b;
    float c;
    std::string d;
  };

  my_struct msg{'a', 5, 3.14f, "Hello"};

  // Serialize
  auto bytes = serialize(s);

  // 0x61 0x05 0xc3 0xf5 0x48 0x40 0x05 0x48 0x65 0x6c 0x6c 0x6f
  // ^^^^ ^^^^ ^^^^^^^^^^^^^^^^^^^ ^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^
  // 'a'  5    3.14                5    "Hello"
  
  // Deserialize
  auto recovered = deserialize<my_struct>(bytes);
  assert(recovered.a == 'a');
  assert(recovered.b == 5);
  assert(recovered.c == 3.14f);
  assert(recovered.d == "Hello");  
}

```
