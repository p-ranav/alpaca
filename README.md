<p align="center">
  <img height="80" src="https://user-images.githubusercontent.com/8450091/187459097-e905ff24-f5ad-44ef-8cdc-e64921921e57.png"/>
</p>

<p align="center">
  <a href="https://github.com/p-ranav/alpaca/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="license"/>
  </a>
  <img src="https://img.shields.io/badge/version-1.0-blue.svg?cacheSeconds=2592000" alt="version"/>
</p>

Pack C++ structs into a compact byte-array without any macros or boilerplate code.

* `alpaca` is header-only serialization library for modern C++, written in C++17
* *No macros or boilerplate, no source code generation, no external dependencies*
* Simple, fast, and easy to use
* Supports basic data types, STL containers, unique pointers, recursive data structures, optionals, variants and more
* Highly configurable at compile time
  - Little endian by default. Configurable to use big endian byte order
  - Variable-length encoding by default for large integer types. Configurable to use fixed-width encoding
  - Optional type hashing and data structure versioning - recursively generates a type hash that is checked during deserialization
  - Optional integrity checking - detects data corruption during deserialization using checksums
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
auto bytes = alpaca::serialize(c);

// Deserialize
std::error_code ec;
auto c_recovered = alpaca::deserialize<Config>(bytes, ec);
if (!ec) {
  // use config
}
```

## Table of Contents

*    [Usage and API](#usage-and-api)
*    [Examples](#examples)
     *    [Fundamental types](#fundamental-types)
     *    [Arrays, Vectors, and Strings](#arrays-vectors-and-strings)
     *    [Maps and Sets](#maps-and-sets)
     *    [Nested Structures](#nested-structures)
     *    [Optional Values](#optional-values)
     *    [Type-safe Unions - Variant Types](#type-safe-unions---variant-types)
     *    [Smart Pointers and Recursive Data Structures](#smart-pointers-and-recursive-data-structures)
*    [Backward and Forward Compatibility](#backward-and-forward-compatibility)
*    [Configuration Options](#configuration-options)
     *    [Endianness](#endianness)
     *    [Fixed or Variable-length Encoding](#fixed-or-variable--length-encoding)
     *    [Data Structure Versioning](#data-structure-versioning)
     *    [Integrity Checking with Checksums](#integrity-checking-with-checksums)
     *    [Macros to Exclude STL Data Structures](#macros-to-exclude-stl-data-structures)
*    [Building, Installing, and Testing](#building-installing-and-testing)
*    [License](#license)

## Usage and API

### Serialization

The `alpaca::serialize(...)` function accepts 2 arguments: an input aggregate class type (typically a `struct`), and an output container, e.g.,  `std::vector<uint8_t>`, `std::array<uint8_T, N>` etc. Serialization will attempt to pack the aggregate input into the container. 

There are two variants to `serialize`, one of which takes an `alpaca::options` for additional configuration:

```cpp
// Serialize a struct T (with N fields) into Container
tmeplate <class T, size_t N, class Container>
auto serialize(const T&, Container&) -> size_t /* bytes_written */;

// Serialize a struct T (with N fields) into Container using options O
tmeplate <options O, class T, size_t N, class Container>
auto serialize(const T&, Container&) -> size_t /* bytes_written */;
```

Examples of valid serialize calls include:

```cpp
struct MyStruct { 
  int value; 
};

// Construct object
MyStruct object{5};

// Serialize to array
std::array<uint8_t, 5> bytes;
auto bytes_written = serialize(object, bytes);

// Serialize to vector
std::vector<uint8_t> bytes;
auto bytes_written = serialize(object, bytes);

// Serialize with options
std::vector<uint8_t> bytes;
constexpr auto OPTIONS = options::fixed_length_encoding | 
                         options::with_version | 
			 options::with_checksum;
auto bytes_written = serialize<OPTIONS>(object, bytes);
```

### Deserialization

The `alpaca::deserialize(...)` function, likewise, accepts 2 arguments: a container like `std::vector<uint8_t>` or `std::array<uint8_t, N>` and an `std::error_code` that will be set in case of error conditions. Deserialization will attempt to unpack the container of bytes into an aggregate class type, returning the class object.

Like `serialize()`, deserialization has two variants, one of which accepts an `alpaca::options` template parameter.  

```cpp
// Deserialize a Container into struct T (with N fields)
template <class T, size_t N, class Container>
auto deserialize(const Container&, std::error_code&) -> T;

// Deserialize a Container into struct T (with N fields) using options O
template <options O, class T, size_t N, class Container>
auto deserialize(const Container&, std::error_code&) -> T;
```

Examples of valid `deserialize` calls include:

```cpp
std::vector<uint8_t> bytes; // or std::array<uint8_t, 256> bytes;

// Default options
std::error_code ec;
auto object = deserialize<MyStruct>(bytes, ec);
if (!ec) {
    // use object
}

// Configured options
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
auto bytes = alpaca::serialize(s); // 9 bytes

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

```cpp
struct MyStruct {
  std::array<int, 3> a;
  std::vector<std::vector<float>> b;
  std::string c;
};

MyStruct s{{1, 2, 3}, {{3.14, 1.61}, {2.71, -1}}, {"Hello"}};

// Serialize
auto bytes = serialize(s); // 28 bytes

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

### Maps and Sets

For associative containers, alpaca supports `std::map`, `std::unordered_map`, `std::set`, and `std::unordered_set`.

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
auto bytes = serialize(s); // 30 bytes

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
auto bytes = serialize(s); // 45 bytes

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
struct MyStruct {
  std::optional<int> a;
  std::optional<float> b;
  std::optional<std::string> c;
  std::optional<std::vector<bool>> d;
};

MyStruct s{5, 3.14f, std::nullopt, std::vector<bool>{true, false, true, false}};

// Serialize
auto bytes = serialize<MyStruct, 4>(s); // 14 bytes
	            // ^^^^^^^^^^^^^ 
	            //    specify the number of fields (4) in struct manually
	            //    alpaca fails at correctly detecting this due to the nature of std::optional

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

For `std::optional<T>`, a leading byte is used to represent if the optional has value

```
has_value?    value (if previous byte is 0x01)         
+----------+  +----+----+----+-----+
|    A1    |  | B1 | B2 | B3 | ... |
+----------+  +----+----+----+-----+
```

### Type-safe Unions - Variant Types

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
auto bytes = serialize(s); // 87 bytes

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

```cpp
template <class T> 
struct Node {
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
    bytes = serialize<my_struct>(s);
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
    bytes = serialize<my_struct>(s);
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

int main() {
  struct my_struct {
    uint16_t id;
  };

  my_struct s { 12345 };
  
  // little endian
  {
    auto bytes = alpaca::serialize(s); // {0x39, 0x30}
  }

  // big endian
  {
    auto bytes = alpaca::serialize<my_struct, std::vector<uint8_t>, alpaca::options::big_endian>(s); // {0x30, 0x39}
  }  
}
```

### Fixed or Variable-length Encoding
	
#### Unsigned integers

* `uint8_t` and `uint16_t` are stored as-is without any encoding. 
* `uint32_t` and `uint64_t` are represented as variable-length quantities (VLQ) with 7-bits for data and 1-bit to represent continuation

<table><thead><tr><th colspan="8">First Octet</th><th colspan="8">Second Octet</th></tr></thead><tbody><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr><tr><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td></tr><tr><td>A</td><td colspan="7">B₀</td><td>A</td><td colspan="7">Bₙ (n &gt; 0)</td></tr></tbody></table>

* If A is 0, then this is the last VLQ octet of the integer. If A is 1, then another VLQ octet follows.

#### Signed integers

* `int8_t` and `int16_t` are stored as-is without any encoding. 
* `int32_t` and `int64_t` are represented as VLQ, similar to the unsigned version. The only difference is that the first VLQ has the sixth bit reserved to indicate whether the encoded integer is positive or negative. Any consecutive VLQ octet follows the general structure.

<table><thead><tr><th colspan="8">First Octet</th><th colspan="8">Second Octet</th></tr></thead><tbody><tr><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td><td>7</td><td>6</td><td>5</td><td>4</td><td>3</td><td>2</td><td>1</td><td>0</td></tr><tr><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td><td>2⁷</td><td>2⁶</td><td>2⁵</td><td>2⁴</td><td>2³</td><td>2²</td><td>2¹</td><td>2⁰</td></tr><tr><td>A</td><td>B</td><td colspan="6">C₀</td><td>B</td><td colspan="7">Cₙ (n &gt; 0)</td></tr></tbody></table>

* If A is 0, then the VLQ represents a positive integer. If A is 1, then the VLQ represents a negative number.
* If B is 0, then this is the last VLQ octet of the integer. If B is 1, then another VLQ octet follows.

### Data Structure Versioning

```cpp
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
```

### Integrity Checking with Checksums

```cpp
struct MyStruct {
  char a;
  uint16_t b;
  float c;
};

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
  auto bytes_written = alpaca::serialize<my_struct, std::vector<uint8_t>, alpaca::options::fixed_length_encoding>(s, bytes);
}
```

## Building, Installing, and Testing

```bash
# Clone
git clone https://github.com/p-ranav/alpaca
cd alpaca

# Build
mkdir build
cd build
cmake -DALPACA_BUILD_TESTS=on -DCMAKE_BUILD_TYPE=Release ..
make

# Test
./test/tests

# Install 
make install
```

## License

The project is available under the [MIT](https://opensource.org/licenses/MIT) license.
