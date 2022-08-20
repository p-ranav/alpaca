
/// Start of Library

#include <cstdint>
#include <boost/pfr.hpp>
#include <type_traits>

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
   vector,  // 0x0c
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

  template<typename T> struct is_vector : public std::false_type {};

  template<typename T, typename A>
  struct is_vector<std::vector<T, A>> : public std::true_type {};

  template<class T>struct tag_t{};
  template<class T>constexpr tag_t<T> tag{};
  namespace is_string {
    template<class T, class...Ts>
    constexpr bool is_stringlike(tag_t<T>, Ts&&...){ return false; }
    template<class T, class A>
    constexpr bool is_stringlike( tag_t<std::basic_string<T,A>> ){ return true; }
    template<class T>
    constexpr bool detect = is_stringlike(tag<T>); // enable ADL extension
  }

  // append the type of value to bytes
  template <typename T, typename U>
  void append_value_type(U& bytes) {
    if constexpr (std::is_same<T, bool>::value) {
      append(type::boolean, bytes);
    }
    else if constexpr (std::is_same<T, char>::value) {
      append(type::uint8, bytes);
    }    
    else if constexpr (std::is_same<T, uint8_t>::value) {
      append(type::uint8, bytes);
    }
    else if constexpr (std::is_same<T, uint16_t>::value) {
      append(type::uint16, bytes);
    }
    else if constexpr (std::is_same<T, uint32_t>::value) {
      append(type::uint32, bytes);
    }
    else if constexpr (std::is_same<T, uint64_t>::value) {
      append(type::uint64, bytes);
    }
    else if constexpr (std::is_same<T, int8_t>::value) {
      append(type::int8, bytes);
    }
    else if constexpr (std::is_same<T, int16_t>::value) {
      append(type::int16, bytes);
    }
    else if constexpr (std::is_same<T, int32_t>::value) {
      append(type::int32, bytes);
    }
    else if constexpr (std::is_same<T, int64_t>::value) {
      append(type::int64, bytes);
    }
    else if constexpr (std::is_same<T, float>::value) {
      append(type::float32, bytes);
    }
    else if constexpr (std::is_same<T, double>::value) {
      append(type::float64, bytes);
    }
    else if constexpr (is_string::detect<T>) {
      append(type::string, bytes);
    }
    else if constexpr (is_vector<T>::value) {
      append(type::vector, bytes);
      append_value_type<typename std::decay<typename T::value_type>::type>(bytes);
    }
  }  

  template <bool save_type_info = true>
  void to_bytes(uint8_t input, std::vector<uint8_t>& bytes) {    
    // type of the value
    if constexpr (save_type_info) {
      append(type::uint8, bytes);
    }
    // width of the value - 1 byte
    constexpr uint8_t width = 1;
    append(width, bytes);
    
    // value
    append(input, bytes);
  }

  template <bool save_type_info = true>  
  void to_bytes(bool input, std::vector<uint8_t>& bytes) {
    // type of the value
    if constexpr (save_type_info) {    
      append(type::boolean, bytes);
    }
    // value
    append(input, bytes);
  }  

  template <bool save_type_info = true>  
  void to_bytes(char input, std::vector<uint8_t>& bytes) {
    to_bytes<save_type_info>(static_cast<uint8_t>(input), bytes);
  }

  template <bool save_type_info = true>  
  void to_bytes(uint16_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint8_t>::max()
        && input >= std::numeric_limits<uint8_t>::min())
    {
      // value can fit in an uint8_t
      to_bytes<save_type_info>(static_cast<uint8_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {      
        append(type::uint16, bytes);
      }
      // width of the value - 2 bytes
      constexpr uint8_t width = 2;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }  

  template <bool save_type_info = true>  
  void to_bytes(uint32_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint16_t>::max()
        && input >= std::numeric_limits<uint16_t>::min())
    {
      // value can fit in an uint16_t
      to_bytes<save_type_info>(static_cast<uint16_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {      
        append(type::uint32, bytes);
      }
      // width of the value - 4 bytes      
      constexpr uint8_t width = 4;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  template <bool save_type_info = true>  
  void to_bytes(uint64_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<uint32_t>::max()
        && input >= std::numeric_limits<uint32_t>::min())
    {
      // value can fit in an uint32_t
      to_bytes<save_type_info>(static_cast<uint32_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {      
        append(type::uint64, bytes);
      }
      // width of the value - 8 bytes      
      constexpr uint8_t width = 8;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  template <bool save_type_info = true>  
  void to_bytes(int8_t input, std::vector<uint8_t>& bytes) {
    // type of the value
    if constexpr (save_type_info) {
      append(type::int8, bytes);
    }
    // width of the value - 1 byte
    constexpr uint8_t width = 1;
    append(width, bytes);
    // value
    append(input, bytes);
  }    

  template <bool save_type_info = true>
  void to_bytes(int16_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int8_t>::max()
        && input >= std::numeric_limits<int8_t>::min())
    {
      // value can find in an int8_t
      to_bytes<save_type_info>(static_cast<int8_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {
        append(type::int16, bytes);
      }
      // width of the value - 2 bytes
      constexpr uint8_t width = 2;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }  

  template <bool save_type_info = true>  
  void to_bytes(int32_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int16_t>::max()
        && input >= std::numeric_limits<int16_t>::min())
    {
      // value can find in an int16_t
      to_bytes<save_type_info>(static_cast<int16_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {
        append(type::int32, bytes);
      }
      // width of the value - 4 bytes      
      constexpr uint8_t width = 4;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  template <bool save_type_info = true>  
  void to_bytes(int64_t input, std::vector<uint8_t>& bytes) {
    if (input <= std::numeric_limits<int32_t>::max()
        && input >= std::numeric_limits<int32_t>::min())
    {
      // value can find in an int32_t
      to_bytes<save_type_info>(static_cast<int32_t>(input), bytes);
    }
    else {
      // type of the value
      if constexpr (save_type_info) {
        append(type::int64, bytes);
      }
      // width of the value - 8 bytes      
      constexpr uint8_t width = 8;
      append(width, bytes);
      // value
      append(input, bytes);
    }
  }

  template <bool save_type_info = true>  
  void to_bytes(const float input, std::vector<uint8_t>& bytes) {
    if constexpr (save_type_info) {
      append(type::float32, bytes);
    }
    append(input, bytes);
  }

  template <bool save_type_info = true>
  void to_bytes(const double input, std::vector<uint8_t>& bytes) {
    if constexpr (save_type_info) {
      append(type::float64, bytes);
    }
    append(input, bytes);
  } 

  template <bool save_type_info = true>  
  void to_bytes(const std::string& input, std::vector<uint8_t>& bytes) {
    if constexpr (save_type_info) {
      append(type::string, bytes);
    }
    
    // size is always some unsigned integer type
    // only save:
    // 1. width (number of bytes in representation)
    // 2. value
    //
    // No need to store type
    to_bytes<false>(input.size(), bytes);

    for (auto& c: input) {
      append(c, bytes);
    }
  }

  template <bool save_type_info, typename T>
  void to_bytes_from_list_type(const T& input, std::vector<uint8_t>& bytes) {
    // type of the value
    if constexpr (save_type_info) {
      append(type::vector, bytes);
      // save type of list::value_type
      append_value_type<typename std::decay<typename T::value_type>::type>(bytes);      
    }
    
    // number of elements in list
    // 
    // size is always some unsigned integer type
    // only save:
    // 1. width (number of bytes in representation)
    // 2. value
    //
    // No need to store type (e.g., uint8)
    to_bytes<false>(input.size(), bytes);

    // value of each element in list
    for (auto& v: input) {
      // check if the value_type is a nested list type
      using decayed_value_type = typename std::decay<decltype(v)>::type;
      if constexpr (is_vector<decayed_value_type>::value) {
	to_bytes_from_list_type<false, decayed_value_type>(v, bytes);
      }
      else {
        to_bytes<false>(v, bytes);
      }
    }
  }
  
}

template<typename T, std::size_t index = 0>
void serialize(T& s, std::vector<uint8_t>& bytes) {
  constexpr static auto max_index = boost::pfr::tuple_size<T>::value;
  if constexpr (index < max_index) {
    using decayed_field_type = typename std::decay<decltype(boost::pfr::get<index>(s))>::type;
    if constexpr (detail::is_vector<decayed_field_type>::value) {
      detail::to_bytes_from_list_type<true, decayed_field_type>(boost::pfr::get<index>(s), bytes);
    }
    else {      
      detail::to_bytes<true>(boost::pfr::get<index>(s), bytes);
    }
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
  std::vector<int> list;
  std::vector<std::vector<char>> list_of_lists;
  std::vector<std::vector<int>> list_of_lists_int;
};


int main() {
  my_struct s{true, {"Hello world!"}, 5, 3.14, {1, 2, 3, 4, 5},
	      {{'a', 'b', 'c'}, {'d', 'e', 'f'}},
	      {{123, 456, 789}, {101112, 131415, 161718}}};
  std::cout << "Original struct size : " << sizeof(s) << " bytes\n";

  std::vector<uint8_t> bytes{};
  serialize<my_struct>(s, bytes);

  bytes.shrink_to_fit();
  std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
  std::cout << "Compression ratio    : " << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
  std::cout << "Space savings        : " << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";  

  for (auto& b: bytes) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)b << " ";
  }
  std::cout << "\n";
  
}
