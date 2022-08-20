
/// Start of Library

#include <cstdint>
#include "boost/pfr.hpp"

namespace detail {

  enum class type : uint8_t
  {
   boolean, // 0x00
   uint8,   // 0x01
   uint16,  // 0x02
   uint32,  // 0x03
   uint64,  // 0x04
   int8,    // 0x05
   int16,   // 0x06
   int32,   // 0x07
   int64,   // 0x08
   float32, // 0x09
   float64, // 0x0a
   string,  // 0x0b
  };

  template <typename T, typename U>
  void append(const T& value, U& bytes) {
    std::copy(static_cast<const char*>(static_cast<const void*>(&value)),
	      static_cast<const char*>(static_cast<const void*>(&value)) + sizeof value,
	      std::back_inserter(bytes));
  }

  template <typename U>
  void append(type type, U& bytes) {
    append(static_cast<uint8_t>(type), bytes);
  }

  void to_bytes(uint8_t input, std::vector<uint8_t>& bytes) {
    // type of the value
    append(type::uint8, bytes);
    // width of the value - 1 byte
    constexpr uint8_t width = 1;
    append(width, bytes);
    // value
    append(input, bytes);
  }

  void to_bytes(bool input, std::vector<uint8_t>& bytes) {
    // type of the value
    append(type::boolean, bytes);
    // value
    append(input, bytes);
  }  

  void to_bytes(char input, std::vector<uint8_t>& bytes) {
    to_bytes(static_cast<uint8_t>(input), bytes);
  }

  void to_bytes(uint16_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint8_t>::max()
        && input >= std::numeric_limits<uint8_t>::min())
    {
      // value can fit in an uint8_t
      to_bytes(static_cast<uint8_t>(input), bytes);
    }
    else {
      // type of the value      
      append(type::uint16, bytes);
      // width of the value - 2 bytes
      constexpr uint8_t width = 2;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }  

  void to_bytes(uint32_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint16_t>::max()
        && input >= std::numeric_limits<uint16_t>::min())
    {
      // value can fit in an uint16_t
      to_bytes(static_cast<uint16_t>(input), bytes);
    }
    else {
      // type of the value
      append(type::uint32, bytes);
      // width of the value - 4 bytes      
      constexpr uint8_t width = 4;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  void to_bytes(uint64_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint32_t>::max()
        && input >= std::numeric_limits<uint32_t>::min())
    {
      // value can fit in an uint32_t
      to_bytes(static_cast<uint32_t>(input), bytes);
    }
    else {
      // type of the value
      append(type::uint64, bytes);
      // width of the value - 8 bytes      
      constexpr uint8_t width = 8;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  void to_bytes(int8_t input, std::vector<uint8_t>& bytes) {
    // type of the value
    append(type::int8, bytes);
    // width of the value - 1 byte
    constexpr uint8_t width = 1;
    append(width, bytes);
    // value
    append(input, bytes);
  }    

  void to_bytes(int16_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int8_t>::max()
        && input >= std::numeric_limits<int8_t>::min())
    {
      // value can find in an int8_t
      to_bytes(static_cast<int8_t>(input), bytes);
    }
    else {
      // type of the value      
      append(type::int16, bytes);
      // width of the value - 2 bytes
      constexpr uint8_t width = 2;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }  

  void to_bytes(int32_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int16_t>::max()
        && input >= std::numeric_limits<int16_t>::min())
    {
      // value can find in an int16_t
      to_bytes(static_cast<int16_t>(input), bytes);
    }
    else {
      // type of the value
      append(type::int32, bytes);
      // width of the value - 4 bytes      
      constexpr uint8_t width = 4;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  void to_bytes(int64_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int32_t>::max()
        && input >= std::numeric_limits<int32_t>::min())
    {
      // value can find in an int32_t
      to_bytes(static_cast<int32_t>(input), bytes);
    }
    else {
      // type of the value
      append(type::int64, bytes);
      // width of the value - 8 bytes      
      constexpr uint8_t width = 8;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  void to_bytes(const float input, std::vector<uint8_t>& bytes) {
    append(type::float32, bytes);
    append(input, bytes);
  }

  void to_bytes(const double input, std::vector<uint8_t>& bytes) {
    append(type::float64, bytes);
    append(input, bytes);
  } 

  void to_bytes(const std::string& input, std::vector<uint8_t>& bytes) {
    append(type::string, bytes);
    to_bytes(input.size(), bytes);
    for (auto& c: input) {
      append(c, bytes);
    }
  }  
  
}

template<typename T, std::size_t index = 0>
void serialize(T& s, std::vector<uint8_t>& bytes) {
  constexpr static auto max_index = boost::pfr::tuple_size<T>::value;
  if constexpr (index < max_index) {
    detail::to_bytes(boost::pfr::get<index>(s), bytes);
    serialize<T, index + 1>(s, bytes);
  }
}

/// Start of Test Code

#include <iostream>

struct my_struct {
  bool flag;
  std::string s;
  int i;
  float f;
};


int main() {
  my_struct s{true, {"Hello world!"}, 5, 3.14};
  std::cout << sizeof(s) << "\n";

  std::cout << "my_struct has " << boost::pfr::tuple_size<my_struct>::value
	    << " fields: " << boost::pfr::io(s) << "\n";

  std::vector<uint8_t> bytes{};
  serialize<my_struct>(s, bytes);

  bytes.shrink_to_fit();
  std::cout << "Serialized to " << bytes.size() << " bytes\n";
  std::cout << "Compression " << (float(bytes.size()) / float(sizeof(s)) * 100.0f) << "%\n";

  for (auto& b: bytes) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)b << " ";
  }
  std::cout << "\n";
  
}
