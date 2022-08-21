/// Start of Library

#include <boost/pfr.hpp>
#include <cstdint>
#include <cstring>
#include <serialize/serialize.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace detail {

template <typename T, typename U> void append(const T &value, U &bytes) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

template <typename U> void append(type type, U &bytes) {
  append(static_cast<uint8_t>(type), bytes);
}

template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template <class T> struct tag_t {};
template <class T> constexpr tag_t<T> tag{};
namespace is_string {
template <class T, class... Ts>
constexpr bool is_stringlike(tag_t<T>, Ts &&...) {
  return false;
}
template <class T, class A>
constexpr bool is_stringlike(tag_t<std::basic_string<T, A>>) {
  return true;
}
template <class T>
constexpr bool detect = is_stringlike(tag<T>); // enable ADL extension
} // namespace is_string

template <typename> struct is_tuple : std::false_type {};

template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

// Forward declarations
template <bool save_type_info, bool attempt_compression>
void to_bytes(uint64_t input, std::vector<uint8_t> &bytes);

template <typename T, std::size_t index>
void save_tuple_value_type(std::vector<uint8_t> &bytes);

template <typename> struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

template <typename T> void save_pair_value_type(std::vector<uint8_t> &bytes);

// append the type of value to bytes
template <typename T, typename U> void append_value_type(U &bytes) {
  if constexpr (std::is_same<T, bool>::value) {
    append(type::boolean, bytes);
  } else if constexpr (std::is_same<T, char>::value) {
    append(type::uint8, bytes);
  } else if constexpr (std::is_same<T, uint8_t>::value) {
    append(type::uint8, bytes);
  } else if constexpr (std::is_same<T, uint16_t>::value) {
    append(type::uint16, bytes);
  } else if constexpr (std::is_same<T, uint32_t>::value) {
    append(type::uint32, bytes);
  } else if constexpr (std::is_same<T, uint64_t>::value) {
    append(type::uint64, bytes);
  } else if constexpr (std::is_same<T, int8_t>::value) {
    append(type::int8, bytes);
  } else if constexpr (std::is_same<T, int16_t>::value) {
    append(type::int16, bytes);
  } else if constexpr (std::is_same<T, int32_t>::value) {
    append(type::int32, bytes);
  } else if constexpr (std::is_same<T, int64_t>::value) {
    append(type::int64, bytes);
  } else if constexpr (std::is_same<T, float>::value) {
    append(type::float32, bytes);
  } else if constexpr (std::is_same<T, double>::value) {
    append(type::float64, bytes);
  } else if constexpr (is_string::detect<T>) {
    append(type::string, bytes);
  } else if constexpr (is_vector<T>::value) {
    append(type::vector, bytes);
    append_value_type<typename std::decay<typename T::value_type>::type>(bytes);
  } else if constexpr (is_tuple<T>::value) {
    append(type::tuple, bytes);
    // save number of tuple elements
    to_bytes<true, true>(std::tuple_size<T>::value, bytes);
    // save type of each tuple element
    save_tuple_value_type<T, 0>(bytes);
  } else if constexpr (is_pair<T>::value) {
    append(type::pair, bytes);
    // save type of each pair element
    save_pair_value_type<T>(bytes);
  } else if constexpr (std::is_class<T>::value) {
    append(type::struct_, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint8_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::uint8, bytes);
  }
  // value
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint16_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<uint8_t>::max() &&
        input >= std::numeric_limits<uint8_t>::min()) {
      // value can fit in an uint8_t
      append(static_cast<uint8_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint32_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<uint16_t>::max() &&
        input >= std::numeric_limits<uint16_t>::min()) {
      // value can fit in an uint16_t
      to_bytes<false, true>(static_cast<uint16_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint64_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<uint32_t>::max() &&
        input >= std::numeric_limits<uint32_t>::min()) {
      // value can fit in an uint32_t
      to_bytes<false, true>(static_cast<uint32_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(bool input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::boolean, bytes);
  }
  // value
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(char input, std::vector<uint8_t> &bytes) {
  to_bytes<save_type_info, attempt_compression>(static_cast<uint8_t>(input),
                                                bytes);
}

template <bool save_type_info>
void to_bytes(int8_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::int8, bytes);
  }
  // value
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int16_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int8_t>::max() &&
        input >= std::numeric_limits<int8_t>::min()) {
      // value can find in an int8_t
      append(static_cast<int8_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int32_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int16_t>::max() &&
        input >= std::numeric_limits<int16_t>::min()) {
      // value can find in an int16_t
      to_bytes<false, true>(static_cast<int16_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int64_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int32_t>::max() &&
        input >= std::numeric_limits<int32_t>::min()) {
      // value can find in an int32_t
      to_bytes<false, true>(static_cast<int32_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const float input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::float32, bytes);
  }
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const double input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::float64, bytes);
  }
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const std::string &input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::string, bytes);
  }

  to_bytes<true, true>(input.size(), bytes);

  for (auto &c : input) {
    append(c, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const char *input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::string, bytes);
  }

  std::size_t size = strlen(input);
  to_bytes<true, true>(size, bytes);

  for (std::size_t i = 0; i < size; ++i) {
    append(input[i], bytes);
  }
}

template <typename T, std::size_t index>
void save_tuple_value_type(std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    using tuple_element_type = std::tuple_element_t<index, T>;
    append_value_type<tuple_element_type>(bytes);
    save_tuple_value_type<T, index + 1>(bytes);
  }
}

template <typename T, std::size_t index>
void save_tuple_value(const T &tuple, std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    to_bytes<false, false>(std::get<index>(tuple), bytes);
    save_tuple_value<T, index + 1>(tuple, bytes);
  }
}

template <bool save_type_info, typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of tuple + each element
  if constexpr (save_type_info) {
    append(type::tuple, bytes);
    // save number of types in tuple
    to_bytes<true, true>(std::tuple_size<T>::value, bytes);
    // save type of each element
    save_tuple_value_type<T, 0>(bytes);
  }

  // value of each element
  save_tuple_value<T, 0>(input, bytes);
}

template <typename T> void save_pair_value_type(std::vector<uint8_t> &bytes) {
  append_value_type<typename T::first_type>(bytes);
  append_value_type<typename T::second_type>(bytes);
}

template <typename T>
void save_pair_value(const T &pair, std::vector<uint8_t> &bytes) {
  to_bytes<false, false>(pair.first, bytes);
  to_bytes<false, false>(pair.second, bytes);
}

template <bool save_type_info, typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of tuple + each element
  if constexpr (save_type_info) {
    append(type::pair, bytes);
    // save type of each element
    save_pair_value_type<T>(bytes);
  }

  // value of each element
  save_pair_value<T>(input, bytes);
}

template <bool save_type_info, typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::vector, bytes);
    // save type of list::value_type
    append_value_type<typename std::decay<typename T::value_type>::type>(bytes);
  }

  to_bytes<true, true>(input.size(), bytes);

  // value of each element in list
  for (const auto &v : input) {
    // check if the value_type is a nested list type
    using decayed_value_type = typename std::decay<decltype(v)>::type;
    if constexpr (is_vector<decayed_value_type>::value) {
      to_bytes_from_list_type<false, decayed_value_type>(v, bytes);
    } else if constexpr (is_tuple<decayed_value_type>::value) {
      to_bytes_from_tuple_type<false, decayed_value_type>(v, bytes);
    } else {
      // dump all the values
      // note: no attempted compression for integer types
      append(v, bytes);
    }
  }
}

} // namespace detail

template <typename T, std::size_t index = 0>
void serialize(T &s, std::vector<uint8_t> &bytes) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;
    // using decayed_field_type =
    //     typename std::decay<decltype(boost::pfr::get<index>(s))>::type;
    if constexpr (detail::is_vector<decayed_field_type>::value) {
      detail::to_bytes_from_list_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);
    } else if constexpr (detail::is_tuple<decayed_field_type>::value) {
      detail::to_bytes_from_tuple_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);

    } else if constexpr (detail::is_pair<decayed_field_type>::value) {
      detail::to_bytes_from_pair_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);
    } else if constexpr (detail::is_string::detect<decayed_field_type>) {
      detail::to_bytes<true, true>(boost::pfr::get<index>(s), bytes);
    } else if constexpr (std::is_class<decayed_field_type>::value) {
      // save nested struct type
      detail::append(detail::type::struct_, bytes);

      // recurse
      serialize<decayed_field_type>(field, bytes);
    } else {
      detail::to_bytes<true, true>(boost::pfr::get<index>(s), bytes);
    }

    // go to next field
    serialize<T, index + 1>(s, bytes);
  }
}

template <typename T> std::vector<uint8_t> serialize(T &s) {
  std::vector<uint8_t> bytes{};
  serialize<T, 0>(s, bytes);
  return bytes;
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
  std::ios_base::fmtflags f(std::cout.flags());

  // Test 1
  {
    my_struct s{true,
                {"Hello world!"},
                5,
                3.14,
                {1, 2, 3, 4, 5},
                {{'a', 'b', 'c'}, {'d', 'e', 'f'}},
                {{123, 456, 789}, {101112, 131415, 161718}}};
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);

    bytes.shrink_to_fit();
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 2
  {
    struct list {
      std::vector<int> values;
    };
    list s;
    for (int i = 0; i < 1E6; ++i) {
      s.values.push_back(i);
    }

    std::vector<uint8_t> bytes{};
    serialize<list>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : "
              << sizeof(s) + s.values.size() * sizeof(s.values[0])
              << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s) + s.values.size() * sizeof(s.values[0])) /
                  float(bytes.size()) * 100.0f)
              << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 3
  {
    struct my_struct {
      const char *str;
    };
    my_struct s;
    s.str = "This is cool";

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : "
              << sizeof(s) + strlen(s.str) * sizeof(s.str[0]) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s) + strlen(s.str) * sizeof(s.str[0])) /
                  float(bytes.size()) * 100.0f)
              << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 4
  {
    struct my_struct {
      std::vector<bool> values;
    };
    my_struct s;
    s.values = {true, false, true, false, true};

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 5
  {
    struct my_struct {
      std::tuple<bool, int, float, std::string, char> values;
    };
    my_struct s;
    s.values = std::make_tuple(true, 5, 3.14, "Hello", 'a');

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 6
  {
    struct my_struct {
      std::vector<std::tuple<bool, int, float, std::string, char>> values;
    };
    my_struct s;
    s.values.push_back(std::make_tuple(true, 5, 3.14, "Hello", 'a'));
    s.values.push_back(std::make_tuple(false, -15, 2.718, "World", 'z'));

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 7
  {
    struct my_struct {
      std::pair<int, float> values;
    };
    my_struct s;
    s.values.first = 5;
    s.values.second = 3.14;

    std::vector<uint8_t> bytes{};
    serialize<my_struct>(s, bytes);
    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";

  // Test 8
  {
    struct my_struct {
      int value;

      struct nested_struct {
        float value;
      };
      nested_struct nested;
    };

    // construct
    my_struct s;
    s.value = 5;
    s.nested.value = 3.14f;

    // serialize
    auto bytes = serialize(s);

    // bytes: 0x0e 0x05 0x1b 0x15 0xc3 0xf5 0x48 0x40

    /*
      field:
        type: 0x0e (int32_as_int8)
        value: 0x05 (value = 5)

      field:
        type: 0x1b (struct)
        field:
          type: 0x15 (float32)
          value: 0xc3 0xf5 0x48 0x40 (value = 3.14f)
    */

    bytes.shrink_to_fit();
    std::cout << "Original struct size : " << sizeof(s) << " bytes\n";
    std::cout << "Serialized to        : " << bytes.size() << " bytes\n";
    std::cout << "Compression ratio    : "
              << (float(sizeof(s)) / float(bytes.size()) * 100.0f) << "%\n";
    std::cout << "Space savings        : "
              << (1 - float(bytes.size()) / float(sizeof(s))) * 100.0f << "%\n";

    for (auto &b : bytes) {
      std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2)
                << (int)b << " ";
    }
    std::cout << "\n";
    std::cout.flags(f);
  }

  std::cout << "\n---\n\n";
}
