<p align="center">
  <img height="80" src="https://user-images.githubusercontent.com/8450091/187056150-3198fd9c-e02a-405c-b3c1-edee60caa156.png"/>
</p>

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

* `alpaca` is header-only serialization library for modern C++, written in C++17
* No macros or boilerplate, no source code generation, no external dependencies
* Simple, fast (see benchmarks), and easy to use
  - Call `auto bytes = serialize(s)` to pack a struct
  - Call `auto obj = deserialize<T>(s, e)` to unpack a struct 
* Optional data structure versioning - detects erroneous usage, e.g., serialize with `structA` and deserialize with `structB`
* Optional integrity checking - detects data corruption during deserialization using CRC32 checksums
* MIT license

```cpp
#include <alpaca/alpaca.h>

// Configuration
struct Camera {
    std::string device;
    std::pair<unsigned, unsigned> resolution;
    std::array<double, 9> K_matrix;
    std::vector<float> distortion_coeffients;

    struct StaticTransform {
        struct Translation { 
            float x, y, z;
        };
        struct Rotation {
            float w, x, y, z;
        };
        Translation translation;
        Rotation rotation;
    };
    StaticTransform static_transform;

    enum class PixelFormat { yuyv, bayer10 };
    PixelFormat pixel_format;
};

int main() {

    // Construct the object
    Camera c{"/dev/video0", {640, 480}, 
             {223.28249888247538, 0.0, 152.30570853111396,
              0.0, 223.8756535707556, 124.5606000035353,
              0.0, 0.0, 1.0},
             {-0.44158343539568284, 0.23861463831967872, 0.0016338407443826572,
               0.0034950038632981604, -0.05239245892096022},
             Camera::StaticTransform{{0.467, -0.258, 0.27},
                                     {0.95, 0.026, 0.083, -0.3}},
             Camera::PixelFormat::yuyv};

    // Serialize
    auto bytes = alpaca::serialize(c);

    // Deserialize
    std::error_code ec;
    auto c_recovered = alpaca::deserialize<Camera>(bytes, ec);
    if (!ec) {
      // use config
    }
}
```

## Table of Contents

*    [Examples](#examples)
     *    [Fundamental types](#fundamental-types)
     *    [Arrays, Vectors, and Strings](#arrays-vectors-and-strings)
     *    [Maps and Sets](#maps-and-sets)
     *    [Nested Structures](#nested-structures)
     *    [Optional Values](#optional-values)
     *    [Type-safe Unions - Variant Types](#type-safe-unions---variant-types)
     *    [Smart Pointers and Recursive Data Structures](#smart-pointers-and-recursive-data-structures)
     *    [Data Structure Versioning](#data-structure-versioning)
     *    [Integrity Checking with CRC32 Checksums](#integrity-checking-with-crc32-checksums)
*    [Supported Types](#supported-types)
*    [Building, Installing, and Testing](#building-installing-and-testing)
*    [License](#license)

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Arrays, Vectors, and Strings

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Maps and Sets

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Nested Structures

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Optional Values

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Type-safe Unions - Variant Types

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  std::map<std::string, 
           std::variant<uint16_t, 
                        std::string, 
                        bool,
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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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
// }
```

### Smart Pointers and Recursive Data Structures

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
  std::error_code ec;
  auto object = deserialize<MyStruct>(bytes, ec);
  if (!ec) {
    // use object
  }
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

### Optional Data Structure Versioning

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  std::vector<uint8_t> bytes;

  // serialize
  {
    struct MyStruct {
      int a;
    };

    MyStruct s{5};
    bytes = serialize<MyStruct, options::with_version>(s);
  }

  // deserialize
  {
    struct MyStruct {
      int a;
      float b;
      char c;
    };

    std::error_code ec;
    auto object = deserialize<MyStruct, options::with_version>(bytes, ec);
    // ec.value() == std::errc::invalid_argument here
  }
}
```

### Optional Integrity Checking with CRC32 Checksums

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

struct MyStruct {
  char a;
  uint16_t b;
  float c;
};

int main() {

  MyStruct s{'m', 54321, -987.654};

  // Serialize and append CRC32 hash
  constexpr auto OPTIONS = options::with_checksum;
  auto bytes = serialize<MyStruct, OPTIONS>(s); // 11 bytes

  // Check CRC32 hash and deserialize
  std::error_code ec;
  auto object = deserialize<MyStruct, OPTIONS>(bytes, ec);
  if (!ec) {
    // use object
  }
}

// bytes:
// {
//   0x6d                   // char 'm'
//   0x31 0xd4              // uint 54321
//   0xdb 0xe9 0x76 0xc4    // float -987.654
//   0xa4 0xf2 0x54 0x76    // crc32 1985278628
// }
//
// crc32({6d,31,d4,db,e9,76,c4}) => 1985278628
// source: https://crccalc.com/
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
# Clone
git clone https://github.com/p-ranav/alpaca
cd alpaca

# Build
mkdir build
cd build
cmake -DALPACA_BUILD_TESTS=on -DALPACA_BUILD_BENCHMARKS=on -DCMAKE_BUILD_TYPE=Release ..
make

# Test
./test/tests

# Install 
make install
```

## Performance Benchmark

```console
pranav@ubuntu:~/dev/alpaca/build$ ./benchmark/alpaca_benchmark --benchmark_counters_tabular=true
2022-08-26T10:23:13-07:00
Running ./benchmark/alpaca_benchmark
Run on (8 X 2592.01 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 256 KiB (x8)
  L3 Unified 9216 KiB (x8)
Load Average: 1.52, 0.72, 0.53
----------------------------------------------------------------------------------------------
Benchmark                                        Time             CPU   Iterations BytesOutput
----------------------------------------------------------------------------------------------
BM_alpaca_serialize/50                       11770 ns        11759 ns        58011      6.913k
BM_alpaca_serialize/100                      22849 ns        22793 ns        31017     13.527k
BM_alpaca_serialize/1000                   1099679 ns      1098702 ns          620    136.928k
BM_alpaca_serialize/10000                  2772087 ns      2766379 ns          249    1.38847M
BM_alpaca_serialize/100000                28011327 ns     27852874 ns           25    13.9007M
BM_alpaca_serialize_with_crc32/50            11768 ns        11742 ns        58682      6.913k
BM_alpaca_serialize_with_crc32/100           22863 ns        22749 ns        30766     13.527k
BM_alpaca_serialize_with_crc32/1000         246552 ns       245566 ns         2852    136.928k
BM_alpaca_serialize_with_crc32/10000       2720921 ns      2710601 ns          257    1.38847M
BM_alpaca_serialize_with_crc32/100000     31508001 ns     31494765 ns           19    13.9007M
BM_alpaca_deserialize/50                     29448 ns        29360 ns        22941      6.913k
BM_alpaca_deserialize/100                    69858 ns        69344 ns         9853     13.527k
BM_alpaca_deserialize/1000                  699213 ns       696371 ns          959    136.928k
BM_alpaca_deserialize/10000                7489914 ns      7451575 ns           90    1.38847M
BM_alpaca_deserialize/100000              87228571 ns     86935481 ns            8    13.9007M
BM_alpaca_deserialize_with_crc32/50          31505 ns        31194 ns        22353      6.917k
BM_alpaca_deserialize_with_crc32/100         74045 ns        73978 ns         9371     13.531k
BM_alpaca_deserialize_with_crc32/1000       735344 ns       730520 ns          935    136.932k
BM_alpaca_deserialize_with_crc32/10000     8032344 ns      8019249 ns           86    1.38847M
BM_alpaca_deserialize_with_crc32/100000   91589497 ns     91075420 ns            7    13.9007M

```

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
