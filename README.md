<p align="center">
  <img height="80" src="https://user-images.githubusercontent.com/8450091/186522141-41207b40-6bca-4d55-ac22-f2a01dd09a15.png"/>
</p>

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

* Header-only library
* No external dependencies
* Requires `C++17`
* MIT License

## Motivating Examples

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
//   0x61,                           // char 'a'
//   0x05,                           // int 5
//   0xb9, 0x60,                     // uint 12345
//   0xc3, 0xf5, 0x48, 0x40,         // float 3.14f
//   0x01                            // bool true
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
//   0x01, 0x02, 0x03,             // array {1, 2, 3}
//   0x02,                         // 2-element vector
//   0x02,                         // 2-element (inner) vector
//   0xc3, 0xf5, 0x48, 0x40,       // vector[0][0] = 3.14
//   0x7b, 0x14, 0xce, 0x3f,       // vector[0][1] = 1.61
//   0x02,                         // 2-element (inner) vector
//   0xa4, 0x70, 0x2d, 0x40,       // vector[1][0] = 2.71
//   0x00, 0x00, 0x80, 0xbf,       // vector[1][1] = -1
//   0x05,                         // start of 5-byte string
//   0x48, 0x65, 0x6c, 0x6c, 0x6f  // string "Hello"
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
//   0x03,                          // 3-element map
//   0x04,                          // start of 4-byte string
//   0x62, 0x6c, 0x75, 0x65,        // string "blue"
//   0x00, 0x00, 0xff,              // tuple {0, 0, 255}
//   0x05,                          // start of 5-byte string
//   0x67, 0x72, 0x65, 0x65, 0x6e,  // string "green"
//   0x00, 0xff, 0x00,              // tuple {0, 255, 0}
//   0x03,                          // 3-byte string
//   0x72, 0x65, 0x64,              // string "red"
//   0xff, 0x00, 0x00,              // tuple {255, 0, 0}
//   0x04,                          // 4-element set
//   0x01, 0x02, 0x03, 0x04         // set {1, 2, 3, 4}
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
//   0x71, 0x3d, 0x0a, 0xd7, 0xa3, 0x90, 0x44, 0x40,  // double 41.13
//   0xcd, 0xcc, 0xcc, 0xcc, 0xcc, 0x6c, 0x52, 0xc0,  // double -73.70
//   0xe0, 0x01,                                      // uint 480
//   0x54, 0x01,                                      // uint 340
//   0x17,                                            // 23-byte string
//   0x68, 0x74, 0x74, 0x70, 0x73, 0x3a, 0x2f, 0x2f,  // "https://"
//   0x66, 0x6f, 0x6f, 0x2f,                          // "foo/"
//   0x62, 0x61, 0x72, 0x2f,                          // "bar/"
//   0x62, 0x61, 0x7a,                                // "baz"
//   0x2e, 0x6a, 0x70, 0x67,                          // "jpg"
//   0x01                                             // enum value 1
// }
```

### Special case: Optional values using `std::optional`

`alpaca` fails at detecting the number of fields in a struct when an `std::optional` is used. So, for this to work correctly, use `serialize<T, N>` and specify the number of fields in the struct (`N`) manually. Don't use `std::optional` in nested structs since (1) there is no way to currently specify the number of fields in nested structs, and (2) the library fails at correctly identifying the number of fields.

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

  detail::print_bytes(bytes);

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
