# serialize

`serialize` is a serialization library written in C++17. 

## Quick Start

Pack C++ structs into a compact byte-array without macros or boilerplate code.

```cpp
#include <cassert>
#include <serialize/serialize.h>

struct MyStruct {
  char a;
  int b;
  float c;
  std::string d;
};

int main() {

  MyStruct s{'a', 5, 3.14f, "Hello"};

  // Serialize
  auto bytes = serialize(s); // 12 bytes
  
  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}
```

The above serialization generates a compact vector of 12 bytes:

```cpp
// bytes
{
  0x61,                           // char 'a'
  0x05,                           // int 5
  0xc3, 0xf5, 0x48, 0x40,         // float 3.14f
  0x05,                           // start of 5-byte string
  0x48, 0x65, 0x6c, 0x6c, 0x6f    // string "Hello"
}
```

## Supported Types

```cpp
uint8_t
uint16_t 
uint32_t 
uint64_t
int8_t 
int16_t 
int32_t 
int64_t
enum class
float
double 
bool
char
std::string
std::optional<T>
std_array<T, N>
std::vector<T>
std::tuple<T, U...>
std::pair<T, U>
std::map<T, U>
std::unordered_map<T, U>
struct
```
