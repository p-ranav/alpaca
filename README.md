<p align="center">
  <img height="80" src="https://user-images.githubusercontent.com/8450091/186522141-41207b40-6bca-4d55-ac22-f2a01dd09a15.png"/>
</p>

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

## Table of Contents

*    [Highlights](#highlights)
*    [Examples](#examples)
     *    [Fundamental types](#fundamental-types)
     *    [Sequence containers: arrays, vectors, and strings](#sequence-containers-arrays-vectors-and-strings)
     *    [Associative containers: map and set](#associative-containers-map-and-set)
     *    [Nested structures](#nested-structures)
     *    [Optional values: `std::optional<T>`](#optional-values)
     *    [Type-safe unions: `std::variant<...>`](#type-safe-unions-stdvariant)
     *    [Pointers: `std::unique_ptr<T>`](#pointers-stdunique_ptrt)
*    [Supported Types](#supported-types)
*    [Building, Installing, and Testing](#building-installing-and-testing)
*    [License](#license)

## Highlights

* Header-only library
* No external dependencies
* Requires `C++17`
* MIT License

## Examples

### Fundamental types

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

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
//   0x61                  // char 'a'
//   0x05                  // int 5
//   0xb9 0x60             // uint 12345
//   0xc3 0xf5 0x48 0x40   // float 3.14f
//   0x01                  // bool true
// }
```

### Sequence containers: arrays, vectors, and strings

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  std::array<int, 3> a;
  std::vector<std::vector<float>> b;
  std::string c;
};

int main() {

  MyStruct s{{1, 2, 3}, {{3.14, 1.61}, {2.71, -1}}, {"Hello"}};

  // Serialize
  auto bytes = serialize(s); // 28 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

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
```

### Associative containers: map and set

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t>> a;
  std::set<int> b;
};

int main() {

  MyStruct s{{{"red", std::make_tuple(255, 0, 0)},
              {"green", std::make_tuple(0, 255, 0)},
              {"blue", std::make_tuple(0, 0, 255)}},
             {1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}};

  // Serialize
  auto bytes = serialize(s); // 30 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

// bytes:
// {
//   0x03                      // 3-element map
//   0x04                      // start of 4-byte string
//   0x62 0x6c 0x75 0x65       // string "blue"
//   0x00 0x00 0xff            // tuple {0, 0, 255}
//   0x05                      // start of 5-byte string
//   0x67 0x72 0x65 0x65 0x6e  // string "green"
//   0x00 0xff 0x00            // tuple {0, 255, 0}
//   0x03                      // 3-byte string
//   0x72 0x65 0x64            // string "red"
//   0xff 0x00 0x00            // tuple {255, 0, 0}
//   0x04                      // 4-element set
//   0x01 0x02 0x03 0x04       // set {1, 2, 3, 4}
// }
```

### Nested structures

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  struct gps {
    double latitude;
    double longitude;
  };
  gps location;

  struct image {
    uint16_t width;
    uint16_t height;
    std::string url;

    struct format {
      enum class type { bayer_10bit, yuyv_422 };
      type type;
    };
    format format;
  };
  image thumbnail;
};

int main() {

  MyStruct s{{41.13, -73.70},
             {480,
              340,
              "https://foo/bar/baz.jpg",
              {MyStruct::image::format::type::yuyv_422}}};

  // Serialize
  auto bytes = serialize(s); // 45 bytes

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

// bytes:
// {
//   0x71 0x3d 0x0a 0xd7 0xa3 0x90 0x44 0x40  // double 41.13
//   0xcd 0xcc 0xcc 0xcc 0xcc 0x6c 0x52 0xc0  // double -73.70
//   0xe0 0x01                                // uint 480
//   0x54 0x01                                // uint 340
//   0x17                                     // 23-byte string
//   0x68 0x74 0x74 0x70 0x73 0x3a 0x2f 0x2f  // "https://"
//   0x66 0x6f 0x6f 0x2f                      // "foo/"
//   0x62 0x61 0x72 0x2f                      // "bar/"
//   0x62 0x61 0x7a                           // "baz"
//   0x2e 0x6a 0x70 0x67                      // ".jpg"
//   0x01                                     // enum value 1
// }
```

### Optional values: `std::optional<T>`

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  std::optional<int> a;
  std::optional<float> b;
  std::optional<std::string> c;
  std::optional<std::vector<bool>> d;
};

int main() {

  MyStruct s{5, 3.14f, std::nullopt, std::vector<bool>{true, false, true, false}};

  // Serialize
  auto bytes = serialize<MyStruct, 4>(s); // 14 bytes
                     // ^^^^^^^^^^^^^ 
                     //    specify the number of fields (4) in struct manually
                     //    alpaca fails at correctly detecting this due to the nature of std::optional

  // Deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

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
```

### Type-safe unions: `std::variant<...>`

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  std::map<std::string, 
           std::variant<uint16_t, std::string, bool,
                        std::vector<std::string>>
          > value;
};

int main() {
  Config s{{{"keepalive", true},
            {"port", uint16_t{8080}},
            {"ip_address", std::string{"192.168.8.1"}},
            {"subscriptions", std::vector<std::string>{"motor_state", "battery_state"}}}};
  
  // serialize
  auto bytes = serialize(s); // 87 bytes
  
  // deserialize
  auto recovered = deserialize<MyStruct>(bytes);
}

// bytes:
// {
//   0x04                                                                // 4-element map
//   0x0a                                                                // 10-byte string
//   0x69 0x70 0x5f 0x61 0x64 0x64 0x72 0x65 0x73 0x73                   // string "ip_address"
//   0x01                                                                // variant index = 1, type string
//   0x0b                                                                // 11-byte string
//   0x31 0x39 0x32 0x2e 0x31 0x36 0x38 0x2e 0x38 0x2e 0x31              // string "192.168.8.1"
//   0x09                                                                // 9-byte string
//   0x6b 0x65 0x65 0x70 0x61 0x6c 0x69 0x76 0x65                        // string "keepalive"
//   0x02                                                                // variant index = 2, type bool
//   0x01                                                                // bool true
//   0x04                                                                // 4-byte string
//   0x70 0x6f 0x72 0x74                                                 // string "port"
//   0x00                                                                // variant index = 0, type uint16_t
//   0x90 0x1f                                                           // uint 8080
//   0x0d                                                                // 13-byte string
//   0x73 0x75 0x62 0x73 0x63 0x72 0x69 0x70 0x74 0x69 0x6f 0x6e 0x73    // string "subscriptions"
//   0x03                                                                // variant index = 3, type vector<string>
//   0x02                                                                // 2-element vector
//   0x0b                                                                // 11-byte string
//   0x6d 0x6f 0x74 0x6f 0x72 0x5f 0x73 0x74 0x61 0x74 0x65              // string "motor_state"
//   0x0d                                                                // 13-byte string
//   0x62 0x61 0x74 0x74 0x65 0x72 0x79 0x5f 0x73 0x74 0x61 0x74 0x65    // string "battery_state"
```

### Pointers: `std::unique_ptr<T>`

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

template <class T> struct Node {
  T data;
  std::unique_ptr<Node<T>> left;
  std::unique_ptr<Node<T>> right;

  Node(const T &data = T(), std::unique_ptr<Node<T>> lhs = nullptr,
       std::unique_ptr<Node<T>> rhs = nullptr)
      : data(data), left(std::move(lhs)), right(std::move(rhs)) {}

  Node(const Node &n) {
    data = n.data;
    left = n.left ? std::unique_ptr<Node<T>>{new Node<T>{*n.left}} : nullptr;
    right = n.right ? std::unique_ptr<Node<T>>{new Node<T>{*n.right}} : nullptr;
  }
};

template <class T>
auto make_node(T const &value, std::unique_ptr<Node<T>> lhs = nullptr,
               std::unique_ptr<Node<T>> rhs = nullptr) {
  return std::unique_ptr<Node<T>>(
      new Node<T>{value, std::move(lhs), std::move(rhs)});
}

int main() {

  /*
    Binary Tree:

          5
         / \
        3   4
       / \
      1   2
  */

  auto const root = make_node(
      5, 
      make_node(
          3, 
          make_node(1), 
          make_node(2)
      ), 
      make_node(4)
  );  

  // serialize
  auto bytes = serialize<Node<int>>(*root); // 15 bytes

  // deserialize
  auto tree = deserialize<Node<int>>(bytes);
}

// bytes:
// {
//   0x05 // root = 5
//   0x01 // 5.has_left = true
//   0x03 // 5.left = 3
//   0x01 // 3.has_left = true
//   0x01 // 3.left = 1
//   0x00 // 1.left = null
//   0x00 // 1.right = null
//   0x01 // 3.has_right = true
//   0x02 // 3.right = 2
//   0x00 // 2.has_left = false
//   0x00 // 2.has_right = false
//   0x01 // 5.has_right = true
//   0x04 // 5.right = 4
//   0x00 // 4.has_left = false
//   0x00 // 4.has_right = false
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
std::unique_ptr<T>
std::variant<T, U...>
struct
```

## Building, Installing, and Testing

```bash
git clone https://github.com/p-ranav/alpaca
cd alpaca
mkdir build && cd build
cmake -DALPACA_BUILD_TESTS=on ..
make
make install
```


## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
