<p align="center">
  <img height="80" src="https://user-images.githubusercontent.com/8450091/188341036-ac47f2cc-1216-4af6-a2ed-7e103c005971.png"/>
</p>

<p align="center">
  <img src="https://github.com/p-ranav/alpaca/workflows/build/badge.svg" alt="build"/>
  <a href="https://en.wikipedia.org/wiki/C%2B%2B17">
    <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="standard"/>
  </a>
  <a href="https://github.com/p-ranav/alpaca/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="license"/>
  </a>
  <img src="https://img.shields.io/badge/version-0.2.1-blue.svg?cacheSeconds=2592000" alt="version"/>
</p>

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

* `alpaca` is header-only serialization library for modern C++, written in C++17
* *No macros or boilerplate, no source code generation, no external dependencies*
* Simple, fast (see [benchmarks](#performance-benchmarks)), and easy to use
* Supports basic data types, STL containers, unique pointers, recursive data structures, optionals, variants and more
* Serialize to C-style arrays, `std::array`, `std::vector`, or even directly to files
* Highly configurable at compile time
  - Little endian by default. Configurable to use big endian byte order
  - Variable-length encoding by default for large integer types. Configurable to use fixed-width encoding
  - Optional type hashing and data structure versioning - recursively generates a type hash that is checked during deserialization
  - Optional integrity checking - detects data corruption during deserialization using checksums
* Samples [here](https://github.com/p-ranav/alpaca/tree/master/samples)
* Experimental Python support with [pybind11](https://github.com/pybind/pybind11)-based wrapper module [pyalpaca](#python-interoperability)
* MIT license

```cpp
#include <alpaca/alpaca.h>

struct Config {
  std::string device;
  std::pair<unsigned, unsigned> resolution;
  std::array<double, 9> K_matrix;
  std::vector<float> distortion_coeffients;
  std::map<std::string, std::variant<uint16_t, std::string, bool>> parameters;
};

// Construct the object
Config c{"/dev/video0", {640, 480}, 
	 {223.28249888247538, 0.0, 152.30570853111396,
	  0.0, 223.8756535707556, 124.5606000035353,
	  0.0, 0.0, 1.0},
	 {-0.44158343539568284, 0.23861463831967872, 0.0016338407443826572,
	  0.0034950038632981604, -0.05239245892096022},
	 {{"start_server", bool{true}},
	  {"max_depth", uint16_t{5}},
	  {"model_path", std::string{"foo/bar.pt"}}}};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(c, bytes);

// Deserialize
std::error_code ec;
auto object = alpaca::deserialize<Config>(bytes, ec);
if (!ec) {
  // use object
}
```

The source for the above example can be found [here](https://github.com/p-ranav/alpaca/blob/master/samples/demo.cpp).

## Table of Contents

*    [Usage and API](#usage-and-api)
     *    [Serialization](#serialization)
     *    [Deserialization](#deserialization)
*    [Examples](#examples)
     *    [Fundamental types](#fundamental-types)
     *    [Arrays, Vectors, and Strings](#arrays-vectors-and-strings)
     *    [Multi-byte Character Strings](#multi-byte-character-strings)
     *    [Maps and Sets](#maps-and-sets)
     *    [Nested Structures](#nested-structures)
     *    [Optional Values](#optional-values)
     *    [Type-safe Unions - Variant Types](#type-safe-unions---variant-types)
     *    [Smart Pointers and Recursive Data Structures](#smart-pointers-and-recursive-data-structures)
     *    [Timestamps and Durations](#timestamps-and-durations)
     *    [Saving/Loading to/from files](#savingloading-tofrom-files)
*    [Backward and Forward Compatibility](#backward-and-forward-compatibility)
*    [Configuration Options](#configuration-options)
     *    [Endianness](#endianness)
     *    [Fixed or Variable-length Encoding](#fixed-or-variable-length-encoding)
     *    [Data Structure Versioning](#data-structure-versioning)
     *    [Integrity Checking with Checksums](#integrity-checking-with-checksums)
     *    [Macros to Exclude STL Data Structures](#macros-to-exclude-stl-data-structures)
*    [Python Interoperability](#python-interoperability)
     *    [Usage](#usage)
     *    [Format String Specification](#format-string-specification)
     *    [Example 1: Serialize and Deserialize in Python](#example-1-serialize-and-deserialize-in-python)
     *    [Example 2: Serialize in C++ and Deserialize in Python](#example-2-serialize-in-c-and-deserialize-in-python)
*    [Performance Benchmarks](#performance-benchmarks)
*    [Building, Installing, and Testing](#building-installing-and-testing)
*    [CMake Integration](#cmake-integration)
*    [Supported Toolchains](#supported-toolchains)
*    [Contributing](#contributing)
*    [License](#license)

## Usage and API

### Serialization

The `alpaca::serialize(...)` function accepts 2 arguments: an input aggregate class type (typically a `struct`), and an output container, e.g.,  `std::vector<uint8_t>`, `std::array<uint8_T, N>` etc. Serialization will attempt to pack the aggregate input into the container. 

There are two variants to `serialize`, one of which takes an `alpaca::options` for additional configuration:

```cpp
// Serialize a struct T (with N fields) into Container
template <class T, size_t N, class Container>
auto serialize(const T&, Container&) -> size_t /* bytes_written */;

// Serialize a struct T (with N fields) into Container using options O
template <options O, class T, size_t N, class Container>
auto serialize(const T&, Container&) -> size_t /* bytes_written */;
```

***NOTE*** Under most circumstances, the number of fields in the struct, `N`, need not be provided. In certain use-cases, e.g., `std::optional`, the user will need to provide this `N` for correct operation. More on this [here](#optional-values).

Examples of valid serialize calls include:

```cpp
struct MyStruct { 
  int value; 
};

// Construct object
MyStruct object{5};
```

```cpp
// Serialize to a C-style array
uint8_t buffer[10];
auto bytes_written = serialize(object, buffer);
```

```cpp
// Serialize to std::array
std::array<uint8_t, 5> bytes;
auto bytes_written = serialize(object, bytes);
```

```cpp
// Serialize to std::vector
std::vector<uint8_t> bytes;
auto bytes_written = serialize(object, bytes);
```

```cpp
// Serialize to file
std::ofstream os;
os.open("foo.bin", std::ios::out | std::ios::binary);
auto bytes_written = serialize(object, os);
```

```cpp
// Serialize with options
std::vector<uint8_t> bytes;
constexpr auto OPTIONS = options::fixed_length_encoding | 
                         options::with_version | 
			 options::with_checksum;
auto bytes_written = serialize<OPTIONS>(object, bytes);
```

### Deserialization

The `alpaca::deserialize(...)` function, likewise, accepts a container like `std::vector<uint8_t>` or `std::array<uint8_t, N>` and an `std::error_code` that will be set in case of error conditions. Deserialization will attempt to unpack the container of bytes into an aggregate class type, returning the class object.

Deserialization from C-style arrays is supported as well, though in this case, the number of bytes to read from the buffer needs to be provided.

Like `serialize()`, deserialization has two variants, one of which accepts an `alpaca::options` template parameter.  

```cpp
// Deserialize a Container into struct T (with N fields)
template <class T, size_t N, class Container>
auto deserialize(Container&, std::error_code&) -> T;

// Deserialize `size` bytes from a Container into struct T (with N fields)
template <class T, size_t N, class Container>
auto deserialize(Container&, const std::size_t, std::error_code&) -> T;

// Deserialize a Container into struct T (with N fields) using options O
template <options O, class T, size_t N, class Container>
auto deserialize(Container&, std::error_code&) -> T;

// Deserialize `size` bytes from a Container into struct T (with N fields) using options O
template <options O, class T, size_t N, class Container>
auto deserialize(Container&, const std::size_t, std::error_code&) -> T;
```

Examples of valid `deserialize` calls include:

```cpp
// Deserialize from flie
std::ifstream is;
is.open("foo.bin", std::ios::in | std::ios::binary);
auto file_size = std::filesystem::file_size("foo.bin");

std::error_code ec;
auto object = deserialize<MyStruct>(is, file_size, ec);
if (!ec) {
  // use object
}
```

```cpp
// Deserialize from std::array or std::vector
// Default options
std::error_code ec;
auto object = deserialize<MyStruct>(bytes, ec);
if (!ec) {
  // use object
}
```

```cpp
// Deserialize from std::array or std::vector
// Custom options
std::error_code ec;
constexpr auto OPTIONS = options::fixed_length_encoding | 
                         options::with_version |
			 options::with_checksum;
auto object = deserialize<OPTIONS, MyStruct>(bytes, ec);
if (!ec) {
  // use object
}
```

## Examples

### Fundamental types

* Fundamental types, including `char`, `bool`, fixed-width integer types like `uint16_t`, and floating-point types are supported by alpaca
* For larger integer types including `int32_t`, alpaca may use variable-length encoding where applicable. If fixed-width encoding is preferred, this can be changed using `options::fixed_width_encoding`.
* By default, alpaca uses little endian for the byte order. This can be changed to use big-endian byte order using `options::big_endian`

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/fundamental_types.cpp)

```cpp
struct MyStruct {
  char a;
  int b;
  uint64_t c;
  float d;
  bool e;
};

MyStruct s{'a', 5, 12345, 3.14f, true};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(s, bytes); // 9 bytes

// bytes:
// {
//   0x61                  // char 'a'
//   0x05                  // int 5
//   0xb9 0x60             // uint 12345
//   0xc3 0xf5 0x48 0x40   // float 3.14f
//   0x01                  // bool true
// }
```

In the above example, `c` is a `uint64_t` but its value is only `5`. Here, alpaca will pack the value in a single byte instead of taking up 8 bytes. This is the default behavior for larger integer types. 

### Arrays, Vectors, and Strings

alpaca supports sequence containers including `std::array`, `std::vector`, and `std::string`. Nested arrays and vectors work seamlessly.

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/arrays_vectors_strings.cpp)

```cpp
struct MyStruct {
  std::array<int, 3> a;
  std::vector<std::vector<float>> b;
  std::string c;
};

MyStruct s{{1, 2, 3}, {{3.14, 1.61}, {2.71, -1}}, {"Hello"}};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(s, bytes); // 28 bytes

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

For `std::string`, the general structure is as follows:

* The first N bytes is a VLQ encoding of the size of the container
* Then, the byte array is simply bytes of data

```
  string length    char array -->
+----+----+-----+  +----+----+-----+
| A1 | A2 | ... |  | B1 | B2 | ... |
+----+----+-----+  +----+----+-----+
```

For `std::vector<T>`, the general structure is as follows:

* The first N bytes is a VLQ encoding of the size of the container
* Then, each value in the vector is encoding accordingly to the rules for value_type `T`

***NOTE*** alpaca also supports `std::list` and `std::deque` with the same structure.

```
   vector size          value1                value2          value3
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
| A1 | A2 | ... |  | B1 | B2 | ... |  | C1 | C2 | C3 | ... |  |...
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
```

For `std::array<T, N>`, since the (1) number of elements and (2) type of element in the array is known (both at serialization and deserialization time), this information is not stored in the byte array. Note that, for this reason, deserialization cannot unpack the bytes into an array of a different size. ***Important***: Make sure to use the same array size on both the serialization and deserialization side. 

The byte array simply includes the encoding for value_type `T` for each value in the array. 

```
     value1             value2                value3          value4
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
| A1 | A2 | ... |  | B1 | B2 | ... |  | C1 | C2 | C3 | ... |  |...
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
```

### Multi-byte Character Strings

alpaca supports the standard `wstring` `u16string`, and `u32string` variants of `std::basic_string` type:

```cpp
struct my_struct {
  std::wstring name;
  std::u16string example;
  std::u32string greeting;
};

std::vector<uint8_t> bytes;

// serialize
{
  my_struct s{L"緋村 剣心", u"This is a string", U"Hello, 世界"};
  serialize(s, bytes);
}

// deserialize
{
  std::error_code ec;
  auto object = deserialize<my_struct>(bytes, ec);
  assert((bool)ec == false);
  assert(object.name == L"緋村 剣心");    
  assert(object.example == u"This is a string");
  assert(object.greeting == U"Hello, 世界");
}
```

### Maps and Sets

For associative containers, alpaca supports `std::map`, `std::unordered_map`, `std::set`, and `std::unordered_set`.

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/maps_and_sets.cpp)

```cpp
struct MyStruct {
  std::map<std::string, std::tuple<uint8_t, uint8_t, uint8_t>> a;
  std::set<int> b;
};

MyStruct s{{{"red", std::make_tuple(255, 0, 0)},
            {"green", std::make_tuple(0, 255, 0)},
            {"blue", std::make_tuple(0, 0, 255)}},
           {1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4}};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(s, bytes); // 30 bytes

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

For `std::map<K, V>` and `std::unordered_map<K, V>`, the structure is similar to sequence containers:

* The first N bytes is a VLQ encoding of the size of the container
* Then, the byte array is `K₁, V₁, K₂, V₂, K₃, V₃, ...` for each key `Kᵢ` and value `Vᵢ` in the map

```
     map size            key1                  value1         key2
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
| A1 | A2 | ... |  | B1 | B2 | ... |  | C1 | C2 | C3 | ... |  |...
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
```

The format for `std::set` and `std::unordered_set` is the same as with `std::vector<T>`:

* The first N bytes is a VLQ encoding of the size of the container
* Then, for each value in the set, is encoding accordingly to the rules for value_type `T`

```
     set size            value1              value2           value3
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
| A1 | A2 | ... |  | B1 | B2 | ... |  | C1 | C2 | C3 | ... |  |...
+----+----+-----+  +----+----+-----+  +----+----+----+-----+  +---
```

### Nested Structures

alpaca works with nested structures and doubly-nested structures seamlessly:

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/nested_structures.cpp)

```cpp
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

MyStruct s{{41.13, -73.70},
           {480,
            340,
            "https://foo/bar/baz.jpg",
            {MyStruct::image::format::type::yuyv_422}}};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(s, bytes); // 45 bytes

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

alpaca has some difficulty with `std::optional`. Due to the implementation of [aggregate_arity](https://github.com/p-ranav/alpaca/blob/master/include/alpaca/detail/aggregate_arity.h), alpaca is unable to correctly determine the number of fields in the struct with optional fields. 

So, to help out, specify the number of fields manually using `serialize<MyStruct, N>(...)`. 

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/optional_values.cpp)

```cpp
struct MyStruct {
  std::optional<int> a;
  std::optional<float> b;
  std::optional<std::string> c;
  std::optional<std::vector<bool>> d;
};

MyStruct s{5, 3.14f, std::nullopt, std::vector<bool>{true, false, true, false}};

// Serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize<MyStruct, 4>(s, bytes); // 14 bytes
	                            // ^^^^^^^^^^^^^ 
	                            //  specify the number of fields (4) in struct manually
	                            //  alpaca fails at correctly detecting 
				    //  this due to the nature of std::optional

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

***NOTE*** Nested structures work as long `std::optional` is not used in the inner struct. This is because (1) alpaca will fail to correctly detect the number of fields in a struct when `std::optional` is used and (2) the API does not provide the means for the user to specify the number of fields in inner structs.

For `std::optional<T>`, a leading byte is used to represent if the optional has value

```
has_value?    value (if previous byte is 0x01)         
+----------+  +----+----+----+-----+
|    A1    |  | B1 | B2 | B3 | ... |
+----------+  +----+----+----+-----+
```

### Type-safe Unions - Variant Types

alpaca also support `std::variant`. Although this is an uncommon data structure for one to use in a messaging framework, it is supported and available. Miscellaneous configuration parameters, like in JSON, can be serialized as variant values and sent to servers. For each variant, a byte of information is used to represent the `variant_index`. As long as the deserialization is performed on the same variant type (where the indices of each type matches exactly), the `std::get<index>` will work fine.

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/variant.cpp)

```cpp
struct MyStruct {
  std::map<std::string, 
           std::variant<uint16_t, 
                        std::string, 
                        bool,
                        std::vector<std::string>>
          > value;
};

Config s{{{"keepalive", true},
          {"port", uint16_t{8080}},
          {"ip_address", std::string{"192.168.8.1"}},
          {"subscriptions", std::vector<std::string>{"motor_state", "battery_state"}}}};
  
// serialize
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(s, bytes); // 87 bytes

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

For `std::variant<T, U, ...>`, the leading bytes represent the index of the variant that is used by the value

```
variant index       value       
+-----------+  +----+----+-----+
|    A1     |  | B1 | B2 | ... |
+-----------+  +----+----+-----+
```

### Smart Pointers and Recursive Data Structures

alpaca supports `std::unique_ptr<T>`. Alpaca does not support raw pointers or shared pointers at the moment. Using unique pointers, recursive data structures, e.g., tree structures, can be easily modeled and serialized. See below for an example:

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/unique_ptr.cpp)

```cpp
template <class T> 
struct Node {
  T data;
  std::unique_ptr<Node<T>> left;
  std::unique_ptr<Node<T>> right;
};

template <class T>
auto make_node(T const &value, std::unique_ptr<Node<T>> lhs = nullptr,
               std::unique_ptr<Node<T>> rhs = nullptr) {
  return std::unique_ptr<Node<T>>(
      new Node<T>{value, std::move(lhs), std::move(rhs)});
}

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
std::vector<uint8_t> bytes;
auto bytes_written = alpaca::serialize(*root, bytes); // 15 bytes

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

For `std::unique_ptr<T>`, a leading byte is used to represent if the pointer is nullptr

```
ptr != null?  value (if previous byte is 0x01)          
+----------+  +----+----+----+-----+
|    A1    |  | B1 | B2 | B3 | ... |
+----------+  +----+----+----+-----+
```

### Timestamps and Durations

alpaca supports `std::chrono::duration<Rep, Period>` type, including `std::chrono::milliseconds` and the like. The `Rep` arithmetic value is serialized and the duration is reconstructed during deserialization

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  struct MyStruct {
    std::chrono::milliseconds period;
  };

  MyStruct s{std::chrono::milliseconds{500}};

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes);

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  // period == 500ms
}
```

Additionally, `std::time_t` can be used to store timestamps. Although not defined by the C standard, this is almost always an integral value holding the number of seconds (not counting leap seconds) since `00:00, Jan 1 1970 UTC`, corresponding to [POSIX time](https://en.wikipedia.org/wiki/Unix_time).

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  struct MyStruct {
    std::time_t timestamp;
  };

  auto timestamp = std::chrono::system_clock::to_time_t(
                        std::chrono::system_clock::now());

  MyStruct s{timestamp};

  constexpr auto OPTIONS = options::big_endian | 
                           options::fixed_length_encoding;

  // Serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize<OPTIONS>(s, bytes);

  // bytes: {0x00 0x00 0x00 0x00 0x63 0x13 0xeb 0x21}

  // Deserialize
  std::error_code ec;
  auto recovered = alpaca::deserialize<OPTIONS, MyStruct>(bytes, ec);

  // timestamp: 1662249761
  //
  // Human time:
  // GMT: Sunday, September 4, 2022 12:02:41 AM
}
```

### Saving/Loading to/from files

alpaca supports directly writing to files instead of using intermediate buffers. Serialize to files using `std::ofstream` and deserialize from files using `std::ifstream` objects. For deserialization, the size of the file must be provided as an argument:

[Source](https://github.com/p-ranav/alpaca/blob/master/samples/write_file.cpp)

```cpp
#include <alpaca/alpaca.h>
#include <filesystem>
using namespace alpaca;

struct GameState {
  int a;
  bool b;
  char c;
  std::string d;
  std::vector<uint64_t> e;
  std::map<std::string, std::array<uint8_t, 3>> f;
};

int main() {

  GameState s{5,
              true,
              'a',
              "Hello World",
              {6, 5, 4, 3, 2, 1},
              {{"abc", {1, 2, 3}}, {"def", {4, 5, 6}}}};

  const auto filename = "savefile.bin";

  {
    // Serialize to file
    std::ofstream os;
    os.open(filename, std::ios::out | std::ios::binary);
    auto bytes_written = serialize(s, os);
    os.close();

    assert(bytes_written == 37);
    assert(std::filesystem::file_size(filename) == 37);
  }

  {
    // Deserialize from file
    auto size = std::filesystem::file_size(filename);
    std::error_code ec;
    std::ifstream is;
    is.open(filename, std::ios::in | std::ios::binary);
    auto recovered = deserialize<GameState>(is, size, ec);
    is.close();

    assert(recovered.a == s.a);
    assert(recovered.b == s.b);
    assert(recovered.c == s.c);
    assert(recovered.d == s.d);
    assert(recovered.e == s.e);
    assert(recovered.f == s.f);
  }
}
```

```console
pranav@ubuntu:~/dev/alpaca/build$ hexdump -C savefile.bin 
00000000  05 01 61 0b 48 65 6c 6c  6f 20 57 6f 72 6c 64 06  |..a.Hello World.|
00000010  06 05 04 03 02 01 02 03  61 62 63 01 02 03 03 64  |........abc....d|
00000020  65 66 04 05 06                                    |ef...|
00000025
```

## Backward and Forward Compatibility

* A change made to a system or technology in such a way that the existing users are unaffected is a ***backward compatible change***. The obvious advantage is that the existing users have a non-time sensitive and a graceful way of upgrading their integrations. On the other hand, a non backward-compatible change breaks the existing integrations and forces the existing users to deal with an immediate fix.
* ***Forward compatibility***, on the other hand is the ability of a system to process input meant for a later version of the system. A message/standard/library/tool (ex: alpaca) supports forward compatibility if an implementation (ex: a service built on alpaca) that uses an older version of the message processes a future version of the message.

### Tips while changing `alpaca` message struct definitions:

* Do not change the order or type of existing fields in the struct. This will break the design considerations meant for backward and forward compatibility.
* Do not remove a field right away if it is not being used anymore. Mark it as deprecated and have a timeline to completely remote it, thereby giving the integrated applications time to flexibly remove the dependency on that field.
* Add new fields for newer implementations and deprecate older fields in a timely way.
* Adding fields is always a safe option as long as you manage them and don't end up with too many of them.

Consider an RPC interaction pattern where a client sends a message to a server. 

Here's the first version of the message struct:

```cpp
struct my_struct {
  int old_field_1;
  float old_field_2;
};
```

### Case 1: Client-side is updated to use a newer version of the message struct

In the scenario where the client side is updated to use a newer version of the struct, which includes a string `new_field_1`. The server side will receive and deserialize this newer version of the message, even though it is compiled to unpack the older version. This is expected to work as long as the newer version simply has more fields. Changes to existing fields, e.g., if `int` was changed to `int8_t`, may or may not work depending on the data.

```cpp
std::vector<uint8_t> bytes;
{
    // client side is updated to use a newer version
    struct my_struct {
        int old_field_1;
        float old_field_2;
        std::string new_field_1;
    };

    my_struct s{5, 3.14f, "Hello"};
    auto bytes_written = alpaca::serialize(s, bytes);
}

{
    // server side is still compiled to deserialize the older version of the struct
    struct my_struct {
        int old_field_1;
        float old_field_2;
    };
    std::error_code ec;
    auto s = deserialize<my_struct>(bytes, ec);
    assert((bool)ec == false);
    assert(s.old_field_1 == 5);
    assert(s.old_field_2 == 3.14f);
}
```

### Case 2: Server-side is updated to use a newer version of the message struct

In this scenario, the server-side is updated to use a newer version of the struct, accepting 3 additional fields: a string, a vector<bool>, and an integer. The client-side is still compiled with the older version of the struct. When the message is deserialized on the server side, the server will construct the newer version of the struct, fill out the fields that are available in the input, and default initialize the rest of the fields. 

```cpp
std::vector<uint8_t> bytes;
{
    // client side is using an old structure
    struct my_struct {
        int old_field_1;
        float old_field_2;
    };

    my_struct s{5, 3.14f};
    auto bytes_written = alpaca::serialize(s, bytes);
}
    
{
    // server side is updated to use a new structure
    struct my_struct {
        int old_field_1;
        float old_field_2;
        std::string new_field_1;
        std::vector<bool> new_field_2;
        int new_field_3;
    };
    std::error_code ec;
    auto s = deserialize<my_struct>(bytes, ec);
    assert((bool)ec == false);
    assert(s.old_field_1 == 5);
    assert(s.old_field_2 == 3.14f);
    assert(s.new_field_1.empty()); // default initialized
    assert(s.new_field_2.size() == 0); // default initialized
    assert(s.new_field_3 == 0); // default initialized
}
```

## Configuration Options
	
### Endianness
	
By default, alpaca uses little endian. This option can be switched using `options::big_endian`

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {
  struct my_struct {
    uint16_t id;
  };

  my_struct s { 12345 };
  
  // little endian
  {
    std::vector<uint8_t> bytes;
    auto bytes_written = serialize(s, bytes); // {0x39, 0x30}
  }

  // big endian
  {
    std::vector<uint8_t> bytes;
    constexpr auto OPTIONS = options::big_endian;
    auto bytes_written = serialize<OPTIONS>(s, bytes); // {0x30, 0x39}
  }  
}
```

### Fixed or Variable-length Encoding

By default, large integer types (32 and 64-bit values), e.g., `int32_t`, `uint64_t` are encoded as variable-length quantities (VLQ). 

This can be changed with `alpaca::options::fixed_length_encoding`. In fixed-length encoding, an `uint32_t` will take up 4 bytes.

```cpp
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {

  struct MyStruct {
    uint32_t value;
  };
  MyStruct s{5};

  // Variable-length encoding
  {
    std::vector<uint8_t> bytes;
    auto bytes_written = serialize(s, bytes); // {0x05}
  }

  // Fixed-length encoding
  {
    std::vector<uint8_t> bytes;
    constexpr auto OPTIONS = options::fixed_length_encoding;
    auto bytes_written = serialize<OPTIONS>(s, bytes); // {0x05, 0x00, 0x00, 0x00}
  }

  // Fixed-length encoding in big endian
  {
    std::vector<uint8_t> bytes;
    constexpr auto OPTIONS = options::fixed_length_encoding |
                             options::big_endian;
    auto bytes_written = serialize<OPTIONS>(s, bytes); // {0x00, 0x00, 0x00, 0x05}
  }
}
```

#### VLQ for Unsigned integers

* `uint8_t` and `uint16_t` are stored as-is without any encoding. 
* `uint32_t` and `uint64_t` are represented as variable-length quantities (VLQ) with 7-bits for data and 1-bit to represent continuation

<table><thead><tr><th colspan="8">First Octet</th><th colspan="8">Second Octet</th></tr></thead><tbody><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr><tr><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td></tr><tr><td>A</td><td colspan="7">B₀</td><td>A</td><td colspan="7">Bₙ (n &gt; 0)</td></tr></tbody></table>

* If A is 0, then this is the last VLQ octet of the integer. If A is 1, then another VLQ octet follows.

#### VLQ for Signed integers

* `int8_t` and `int16_t` are stored as-is without any encoding. 
* `int32_t` and `int64_t` are represented as VLQ, similar to the unsigned version. The only difference is that the first VLQ has the sixth bit reserved to indicate whether the encoded integer is positive or negative. Any consecutive VLQ octet follows the general structure.

<table><thead><tr><th colspan="8">First Octet</th><th colspan="8">Second Octet</th></tr></thead><tbody><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr><tr><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td></tr><tr><td>A</td><td>B</td><td colspan="6">C₀</td><td>B</td><td colspan="7">Cₙ (n &gt; 0)</td></tr></tbody></table>

* If A is 0, then the VLQ represents a positive integer. If A is 1, then the VLQ represents a negative number.
* If B is 0, then this is the last VLQ octet of the integer. If B is 1, then another VLQ octet follows.

### Data Structure Versioning

alpaca provides a type-hashing mechanism to encode the version the aggregate class type as a `uint32_t`. This hash can be added to the output using `alpaca::options::with_version`.  The type hash includes the number of fields in the struct, the `sizeof(T)` for the struct, an ordered list of the type of each field. This information is encoded into a bytearray and then a checksum is generated for those bytes. 

During deserialization, the same type hash is calculated and compared against the input. In case of a mismatch, the error code is set. 

```cpp
std::vector<uint8_t> bytes;

// serialize
{
  struct MyStruct {
    int a;
  };

  MyStruct s{5};
  std::vector<uint8_t> bytes;
  auto bytes_written = serialize<options::with_version>(s, bytes);
}

// deserialize
{
  struct MyStruct {
    int a;
    float b;
    char c;
  };

  std::error_code ec;
  auto object = deserialize<options::with_version, MyStruct>(bytes, ec);
  // ec.value() == std::errc::invalid_argument here
}
```

### Integrity Checking with Checksums
	
In addition to type hashing, checksums can be added to the end of the output using `options::with_checksum`. This will generate a `CRC32` checksum for all the bytes in the serialized output and then append the four additional bytes to the end of the output. 

```cpp
struct MyStruct {
  char a;
  uint16_t b;
  float c;
};

MyStruct s{'m', 54321, -987.654};
	
std::vector<uint8_t> bytes;

// Serialize and append CRC32 hash
constexpr auto OPTIONS = options::with_checksum;
auto bytes_written = serialize<OPTIONS>(s, bytes); // 11 bytes

// Check CRC32 hash and deserialize
std::error_code ec;
auto object = deserialize<OPTIONS, MyStruct>(bytes, ec);
if (!ec) {
  // use object
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

### Macros to Exclude STL Data Structures

alpaca includes headers for a number of STL containers and classes. As this can affect the compile time of applications, define any of the following macros to remove support for particular data structures. 

```cpp
#define ALPACA_EXCLUDE_SUPPORT_STD_ARRAY
#define ALPACA_EXCLUDE_SUPPORT_STD_CHRONO
#define ALPACA_EXCLUDE_SUPPORT_STD_DEQUE
#define ALPACA_EXCLUDE_SUPPORT_STD_LIST
#define ALPACA_EXCLUDE_SUPPORT_STD_MAP
#define ALPACA_EXCLUDE_SUPPORT_STD_OPTIONAL
#define ALPACA_EXCLUDE_SUPPORT_STD_SET
#define ALPACA_EXCLUDE_SUPPORT_STD_STRING
#define ALPACA_EXCLUDE_SUPPORT_STD_TUPLE
#define ALPACA_EXCLUDE_SUPPORT_STD_PAIR
#define ALPACA_EXCLUDE_SUPPORT_STD_UNIQUE_PTR
#define ALPACA_EXCLUDE_SUPPORT_STD_UNORDERED_MAP
#define ALPACA_EXCLUDE_SUPPORT_STD_UNORDERED_SET
#define ALPACA_EXCLUDE_SUPPORT_STD_VARIANT
#define ALPACA_EXCLUDE_SUPPORT_STD_VECTOR
```

Here's an example that only uses `std::vector`, `std::unordered_map`, and `std::string`

```cpp
#define ALPACA_EXCLUDE_SUPPORT_STD_ARRAY
#define ALPACA_EXCLUDE_SUPPORT_STD_MAP
#define ALPACA_EXCLUDE_SUPPORT_STD_UNIQUE_PTR
#define ALPACA_EXCLUDE_SUPPORT_STD_OPTIONAL
#define ALPACA_EXCLUDE_SUPPORT_STD_SET
#define ALPACA_EXCLUDE_SUPPORT_STD_TUPLE
#define ALPACA_EXCLUDE_SUPPORT_STD_UNORDERED_SET
#define ALPACA_EXCLUDE_SUPPORT_STD_PAIR
#define ALPACA_EXCLUDE_SUPPORT_STD_VARIANT
#include <alpaca/alpaca.h>
using namespace alpaca;

int main() {
  struct my_struct {
    uint16_t id;
    std::vector<char> alphabet;
    std::unordered_map<std::string, int> config;
  };

  my_struct s {12345,
	       {'a', 'b', 'c'},
	       {{"x", -20}, {"y", 45}}};
  
  std::vector<std::uint8_t> bytes;
  auto bytes_written = serialize<options::fixed_length_encoding>(s, bytes);
}
```

## Python Interoperability

alpaca comes with an experimental [pybind11](https://github.com/pybind/pybind11)-based Python wrapper called `pyalpaca`. To build this wrapper, include the option `-DALPACA_BUILD_PYTHON_LIB=on` with `cmake`. 

Instead of providing a `struct` type, the user will provide a string specification of the fields. This is inspired by the standard Python [struct](https://docs.python.org/3/library/struct.html) module.

### Usage

```python
# Serialize
def serialize(format_string, list_of_values) -> bytes

# Deserialize
def deserialize(format_string, bytes) -> list_of_values
```

### Format String Specification

| Code          | Type                       |
|--------------:|---------------------------:|
|           `?` |                     `bool` |
|           `c` |                     `char` |
|           `b` |                   `int8_t` |
|           `B` |                  `uint8_t` |
|           `h` |                  `int16_t` |
|           `H` |                 `uint16_t` |
|           `i` |                  `int32_t` |
|           `I` |                 `uint32_t` |
|           `q` |                  `int64_t` |
|           `Q` |                 `uint64_t` |
|           `f` |                    `float` |
|           `d` |                   `double` |
|           `N` |              `std::size_t` |
|           `s` |              `std::string` |
|         `[T]` |           `std::vector<T>` |
|        `[NT]` |          `std::array<N,T>` |
|       `{K:V}` | `std::unordered_map<K, V>` |
|         `{T}` |    `std::unordered_set<T>` |
| `(T, U, ...)` |    `std::tuple<T, U, ...>` |

### Example 1: Serialize and Deserialize in Python

Once the wrapper is built, simply add it to `PYTHONPTAH` and `import pyalpaca`.

```python
import pyalpaca

# Create format string
format = '?cifs[i][[d]][3c]{c:i}{I}(cif)(s(dI))'

# Construct object
object = [
    False, 
    'a', 
    5, 
    3.14, 
    "Hello World!",
    [0, 1, 2, 3], 
    [[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]],
    ['a', 'b', 'c'],
    {'a': 5, 'b': 19},
    {1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 6},
    ('a', 45, 2.718),
    ("Hello", (39.456, 21))
]

# Serialize
bytes = pyalpaca.serialize(format, object)

# Print it
print("Bytes:")
hex_values = ["0x{:02x}".format(b) for b in bytes]
for i, h in enumerate(hex_values):
    if i > 0 and i % 8 == 0:
        print("\n  ", end="")
    elif i == 0 and i % 8 == 0:
        print("  ", end="")
    print(h, end=" ")
print()

# Deserialize
recovered = pyalpaca.deserialize(format, bytes)

# Print it
print("\nDeserialized:\n[ ")
for i in recovered:
    print("    " + str(i) + ",")
print("]")
```

```console
pranav@ubuntu:~/dev/alpaca/build/python$ python3 test.py
Bytes:
  0x00 0x61 0x05 0xc3 0xf5 0x48 0x40 0x0c 
  0x48 0x65 0x6c 0x6c 0x6f 0x20 0x57 0x6f 
  0x72 0x6c 0x64 0x21 0x04 0x00 0x01 0x02 
  0x03 0x03 0x02 0x9a 0x99 0x99 0x99 0x99 
  0x99 0xf1 0x3f 0x9a 0x99 0x99 0x99 0x99 
  0x99 0x01 0x40 0x02 0x66 0x66 0x66 0x66 
  0x66 0x66 0x0a 0x40 0x9a 0x99 0x99 0x99 
  0x99 0x99 0x11 0x40 0x02 0x00 0x00 0x00 
  0x00 0x00 0x00 0x16 0x40 0x66 0x66 0x66 
  0x66 0x66 0x66 0x1a 0x40 0x61 0x62 0x63 
  0x02 0x61 0x05 0x62 0x13 0x06 0x01 0x02 
  0x03 0x04 0x05 0x06 0x61 0x2d 0xb6 0xf3 
  0x2d 0x40 0x05 0x48 0x65 0x6c 0x6c 0x6f 
  0xee 0x7c 0x3f 0x35 0x5e 0xba 0x43 0x40 
  0x15 

Deserialized:
[ 
    False,
    a,
    5,
    3.140000104904175,
    Hello World!,
    [0, 1, 2, 3],
    [[1.1, 2.2], [3.3, 4.4], [5.5, 6.6]],
    ['a', 'b', 'c'],
    {'a': 5, 'b': 19},
    {1, 2, 3, 4, 5, 6},
    ('a', 45, 2.7179999351501465),
    ('Hello', (39.456, 21)),
]
```

### Example 2: Serialize in C++ and Deserialize in Python

Serialize a `GameState` to file in C++

```cpp
#include <alpaca/alpaca.h>
#include <filesystem>
using namespace alpaca;

struct GameState {
  int a;
  bool b;
  char c;
  std::string d;
  std::vector<uint64_t> e;
  std::map<std::string, std::array<uint8_t, 3>> f;
};

int main() {

  GameState s{5,
              true,
              'a',
              "Hello World",
              {6, 5, 4, 3, 2, 1},
              {{"abc", {1, 2, 3}}, {"def", {4, 5, 6}}}};

  const auto filename = "savefile.bin";

  {
    // Serialize to file
    std::ofstream os;
    os.open(filename, std::ios::out | std::ios::binary);
    auto bytes_written = serialize(s, os);
    os.close();

    assert(bytes_written == 37);
    assert(std::filesystem::file_size(filename) == 37);
  }
}
```

```console
pranav@ubuntu:~/dev/alpaca/build/python$ hexdump -C savefile.bin 
00000000  05 01 61 0b 48 65 6c 6c  6f 20 57 6f 72 6c 64 06  |..a.Hello World.|
00000010  06 05 04 03 02 01 02 03  61 62 63 01 02 03 03 64  |........abc....d|
00000020  65 66 04 05 06                                    |ef...|
00000025
```

Now one can deserialize this file in Python by simply adding the format string to match the C++ struct:

```python
import pyalpaca

# Read file
with open("savefile.bin", "rb") as file:
    bytes = file.read()

    # Format string
    format = "i?cs[Q]{s:[3B]}"

    # Deserialize
    recovered = pyalpaca.deserialize(format, bytes)

    # Print it
    print("\nDeserialized:\n[ ")
    for i in recovered:
        print("    " + str(i) + ",")
    print("]")
```

```console
pranav@ubuntu:~/dev/alpaca/build/python$ python3 test.py 

Deserialized:
[ 
    5,
    True,
    a,
    Hello World,
    [6, 5, 4, 3, 2, 1],
    {'abc': [1, 2, 3], 'def': [4, 5, 6]},
]
```

## Performance Benchmarks
	
Last updated: 2022-09-13

All tests benchmark the following properties (time or size):

* **Serialize**: serialize data into a buffer
* **Deserialize**: deserializes a buffer into a C++ struct object
* **Size**: the size of the buffer when serialized

### System Details

| Type            | Value                                                                                                     |
| --------------- | --------------------------------------------------------------------------------------------------------- |
| Processor       | 11th Gen Intel(R) Core(TM) i9-11900KF @ 3.50GHz   3.50 GHz                                                |
| Installed RAM   | 32.0 GB (31.9 GB usable)                                                                                  |
| SSD             | [ADATA SX8200PNP](https://www.adata.com/upload/downloadfile/Datasheet_XPG%20SX8200%20Pro_EN_20181017.pdf) |
| OS              | Ubuntu 20.04 LTS running on VMWare Player                                                                 |
| C++ Compiler    | g++ (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0                                                                 |

The tests cover three example scenarios:

* **[Log](https://github.com/p-ranav/alpaca/blob/master/benchmark/log.h)**: This data set is composed of HTTP request logs that are small and contain many strings.
* **[Triangle Mesh](https://github.com/p-ranav/alpaca/blob/master/benchmark/mesh.h)**: This data set is a single mesh. The mesh contains an array of triangles, each of which has three vertices and a normal vector.
* **[Minecraft Save](https://github.com/p-ranav/alpaca/blob/master/benchmark/minecraft_save.h)**: This data set is composed of Minecraft player saves that contain highly structured data.

| Test Name      |             Count | Serialize | Deserialize |       Size |
|----------------|------------------:|----------:|------------:|-----------:|
| Log            | 10,000 logs       | 432.95 us |     2.27 ms |  850.52 KB |
| Triangle Mesh  | 125,000 triangles | 777.96 us |     2.37 ms |    6.00 MB |
| Minecraft Save | 50 players        |  71.54 us |   321.10 us |  149.05 KB |

## Building, Installing, and Testing

```bash
# Clone
git clone --recurse-submodules https://github.com/p-ranav/alpaca
cd alpaca

# Build
mkdir build
cd build
cmake -DALPACA_BUILD_TESTS=on \
      -DALPACA_BUILD_BENCHMARKS=on \
      -DALPACA_BUILD_SAMPLES=on \
      -DCMAKE_BUILD_TYPE=Release ..
make

# Test
./test/tests

# Install 
make install
```

## CMake Integration

Use the latest alpaca in your CMake project without copying any content.

```
cmake_minimum_required(VERSION 3.11)

PROJECT(myproject)

# fetch latest argparse
include(FetchContent)
FetchContent_Declare(
    alpaca
    GIT_REPOSITORY https://github.com/p-ranav/alpaca.git
)
FetchContent_MakeAvailable(alpaca)

add_executable(myproject main.cpp)
target_link_libraries(myproject alpaca)
```

## Supported Toolchains

alpaca has been tested on the following toolchains (see [actions](https://github.com/p-ranav/alpaca/actions)).

| Compiler                      | Standard Library | Test Environment       |
| :---------------------------- | :--------------- | :--------------------- |
| AppleClang >= 13.0.0.13000029 | libc++           | Xcode 13.2.1           |
| Clang >= 11.0.0               | libstdc++        | Ubuntu 20.04           |
| GCC >= 9.4.0                  | libstdc++        | Ubuntu 20.04           |
| MSVC >= 19.33.31629.0         | Microsoft STL    | Visual Studio 17 2022  |

## Contributing

Feel free to contribute to this project. Issues and PRs welcome.

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
