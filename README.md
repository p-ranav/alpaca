# structbyte

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

* Header-only library
* No external dependencies
* Requires `C++17`
* MIT License

## Motivating Examples

### Fundamental types

```cpp
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
  auto bytes = serialize(s); // 9 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

// bytes:
// {
//   0x61,                           // char 'a'
//   0x05,                           // int 5
//   0xb9, 0x60,                     // 2-byte uint 12345
//   0xc3, 0xf5, 0x48, 0x40,         // float 3.14f
//   0x01                            // bool true
// }
```

### Sequence containers: arrays, vectors, and strings

```cpp
#include <structbyte/structbyte.h>
using namespace structbyte;

struct MyStruct {
  std::array<int, 3> a;
  std::vector<std::vector<float>> b;
  std::string c;
};

int main() {

  MyStruct s{{1, 2, 3},
	     {{3.14, 1.61}, {2.71, -1}}
	     "Hello"};

  // Serialize
  auto bytes = serialize(s); // 28 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
} 

// bytes:
// {
//   0x01, 0x02, 0x03,             // array {1, 2, 3}
//   0x02,                         // vector size = 2
//   0x02,                         // start of inner vector (size = 2)
//   0xc3, 0xf5, 0x48, 0x40,       // vector[0][1] = 3.14
//   0x7b, 0x14, 0xce, 0x3f,       // vector[0][1] = 1.61
//   0x02,                         // sub-vector size = 2
//   0xa4, 0x70, 0x2d, 0x40,       // vector[1][0] = 2.71
//   0x00, 0x00, 0x80, 0xbf        // vector[1][0] = -1
//   0x05,                         // start of 5-byte string
//   0x48, 0x65, 0x6c, 0x6c, 0x6f  // string "Hello"
// }
```

### Associative containers: map and set

```cpp
#include <structbyte/structbyte.h>
using namespace structbyte;

struct MyStruct {
  std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t>> a;
  std::set<int> b;
};

int main() {

  MyStruct s{
             {
              {"red", std::make_tuple(255, 0, 0)},
              {"green", std::make_tuple(0, 255, 0)},
              {"blue", std::make_tuple(0, 0, 255)}
             },
             {1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}
  };

  // Serialize                                                                                           
  auto bytes = serialize(s); // 33 bytes                                                                 

  // Deserialize                                                                                         
  auto recovered = deserialize<MyStruct>(bytes);
}

// bytes:                                                                                                
// {                                                                                                     
//   0x03                          // 3-element map                                                      
//   0x04                          // start of 4-byte string                                             
//   0x62, 0x6c, 0x75, 0x65        // string "blue"                                                      
//   0x00, 0x00, 0xff, 0x01        // tuple {0, 0, 255}                                                  
//   0x05                          // start of 5-byte string                                             
//   0x67 0x72 0x65 0x65 0x6e      // string "green"                                                     
//   0x00 0xff 0x01, 0x00          // tuple {0, 255, 0}                                                  
//   0x03                          // 3-byte string                                                      
//   0x72 0x65 0x64                // string "red"                                                       
//   0xff 0x01 0x00, 0x00          // tuple {255, 0, 0}                                                  
//   0x04                          // 4-element set                                                      
//   0x01 0x02 0x03 0x04           // set {1, 2, 3, 4}                                                   
// } 
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
std::set<T>
std::unordered_set<T>
struct
```
