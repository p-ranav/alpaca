#pragma once
#include <alpaca/alpaca.h>
#include <cctype>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace py = pybind11;

namespace alpaca {

namespace python {

// Errors in the format string or in the values given for it. pybind11 maps
// std::invalid_argument to Python's ValueError.
struct format_error : std::invalid_argument {
  using std::invalid_argument::invalid_argument;
};

// Errors in the bytes being deserialized (truncated input, bad checksum).
// pybind11 maps std::runtime_error to Python's RuntimeError.
struct decode_error : std::runtime_error {
  using std::runtime_error::runtime_error;
};

template <options O>
std::vector<uint8_t> serialize(const std::string &format, const py::list &args);

// Parse the decimal size that follows '[' in an array spec such as "[3B]".
inline std::size_t get_size(const std::string &format, std::size_t &index) {
  std::string chars{""};
  while (index < format.size() &&
         std::isdigit(static_cast<unsigned char>(format[index]))) {
    chars += format[index];
    index++;
  }
  if (chars.empty()) {
    throw format_error("expected an array size at index " +
                       std::to_string(index));
  }
  return std::stoull(chars);
}

// Return the text between a matching pair of brackets starting at format[index]
// (which must be `open`), leaving index on the closing bracket.
inline std::string get_bracketed(const std::string &format, std::size_t &index,
                                 char open, char close) {
  std::string result{};
  if (index >= format.size() || format[index] != open) {
    throw format_error(std::string("expected '") + open + "' at index " +
                       std::to_string(index));
  }
  // get past the opening bracket
  index++;
  std::size_t level = 1;
  while (index < format.size()) {
    const char c = format[index];
    if (c == close) {
      level -= 1;
      if (level == 0) {
        break;
      }
    } else if (c == open) {
      level += 1;
    }
    result += c;
    index++;
  }
  if (level != 0) {
    throw format_error(std::string("unbalanced '") + open + "' in format '" +
                       format + "'");
  }
  if (result.empty()) {
    throw format_error(std::string("expected a type inside '") + open + close +
                       "' at index " + std::to_string(index));
  }
  return result;
}

// Inside "[...]": an array when it starts with a size ("[3B]"), else a vector.
inline std::string get_value_type_array(const std::string &format,
                                        std::size_t &index) {
  // index is on the first character after the size
  std::string result{};
  std::size_t level = 1;
  while (index < format.size()) {
    const char c = format[index];
    if (c == ']') {
      level -= 1;
      if (level == 0) {
        break;
      }
    } else if (c == '[') {
      level += 1;
    }
    result += c;
    index++;
  }
  if (level != 0 || result.empty()) {
    throw format_error("bad array spec in format '" + format + "'");
  }
  return result;
}

inline std::string get_value_type_vector(const std::string &format,
                                         std::size_t &index) {
  return get_bracketed(format, index, '[', ']');
}

inline std::string get_value_type_map(const std::string &format,
                                      std::size_t &index) {
  return get_bracketed(format, index, '{', '}');
}

inline std::string get_value_type_tuple(const std::string &format,
                                        std::size_t &index) {
  return get_bracketed(format, index, '(', ')');
}

// "{K:V}" is a map, "{T}" is a set: the first top-level ':' decides.
inline bool is_map_spec(const std::string &key_value_type, std::size_t &colon) {
  std::size_t level = 0;
  for (std::size_t i = 0; i < key_value_type.size(); ++i) {
    const char c = key_value_type[i];
    if (c == '[' || c == '{' || c == '(') {
      level++;
    } else if (c == ']' || c == '}' || c == ')') {
      level--;
    } else if (c == ':' && level == 0) {
      colon = i;
      return true;
    }
  }
  return false;
}

template <typename T>
T cast_value(const py::handle &value, const char *type_name,
             std::size_t field) {
  try {
    return value.cast<T>();
  } catch (const py::cast_error &) {
    throw format_error("value " + std::to_string(field) +
                       " is not convertible to " + type_name);
  }
}

template <options O>
void append_serialized(const std::string &format, const py::list &values,
                       std::vector<uint8_t> &result, std::size_t &byte_index) {
  auto serialized = serialize<O>(format, values);
  for (auto &b : serialized) {
    result.push_back(b);
  }
  byte_index = result.size();
}

template <options O>
void parse_array(const std::string &format, const py::handle &value,
                 std::size_t &index, std::vector<uint8_t> &result,
                 std::size_t &byte_index, std::size_t field) {
  // get past '['
  index++;

  const auto size = get_size(format, index);
  auto list = py::cast<py::list>(value);
  const std::size_t input_size = py::len(list);
  if (input_size != size) {
    throw format_error("value " + std::to_string(field) + ": expected " +
                       std::to_string(size) + " array elements, got " +
                       std::to_string(input_size));
  }

  // No need to serialize the size
  auto value_type = get_value_type_array(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }
  append_serialized<O>(value_format_list, list, result, byte_index);
}

template <options O>
void parse_vector(const std::string &format, const py::handle &value,
                  std::size_t &index, std::vector<uint8_t> &result,
                  std::size_t &byte_index) {
  auto list = py::cast<py::list>(value);

  // serialize size the way alpaca does for std::vector: as
  // size_t_serialized_type
  const auto size = static_cast<detail::size_t_serialized_type>(py::len(list));
  detail::to_bytes_router<O>(size, result, byte_index);

  auto value_type = get_value_type_vector(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }
  append_serialized<O>(value_format_list, list, result, byte_index);
}

template <options O>
void parse_unordered_map(const std::string &key_value_type, std::size_t colon,
                         const py::handle &value, std::vector<uint8_t> &result,
                         std::size_t &byte_index) {
  auto dict = py::cast<py::dict>(value);

  // serialize size
  const auto size = static_cast<detail::size_t_serialized_type>(py::len(dict));
  detail::to_bytes_router<O>(size, result, byte_index);

  auto key_type = key_value_type.substr(0, colon);
  auto value_type = key_value_type.substr(colon + 1);

  for (auto item : dict) {
    py::list keys;
    keys.append(item.first);
    append_serialized<O>(key_type, keys, result, byte_index);

    py::list values;
    values.append(item.second);
    append_serialized<O>(value_type, values, result, byte_index);
  }
}

template <options O>
void parse_unordered_set(const std::string &value_type, const py::handle &value,
                         std::vector<uint8_t> &result,
                         std::size_t &byte_index) {
  auto set = py::cast<py::set>(value);

  // serialize size
  const auto size = static_cast<detail::size_t_serialized_type>(py::len(set));
  detail::to_bytes_router<O>(size, result, byte_index);

  for (auto item : set) {
    py::list items;
    items.append(item);
    append_serialized<O>(value_type, items, result, byte_index);
  }
}

template <options O>
void parse_tuple(const std::string &value_type, const py::handle &value,
                 std::vector<uint8_t> &result, std::size_t &byte_index) {
  const auto tuple = py::cast<py::tuple>(value);
  auto list = py::cast<py::list>(tuple);
  append_serialized<O>(value_type, list, result, byte_index);
}

template <options O>
std::vector<uint8_t> serialize(const std::string &format,
                               const py::list &args) {
  std::vector<uint8_t> result;
  std::size_t byte_index = 0;
  std::size_t index = 0;
  std::size_t field = 0;

  for (auto it = args.begin(); it != args.end(); ++it, ++field) {
    if (index >= format.size()) {
      throw format_error("format '" + format + "' describes " +
                         std::to_string(field) + " fields but " +
                         std::to_string(py::len(args)) + " values were given");
    }
    const char code = format[index];
    const py::handle value = *it;
    switch (code) {
    case '?':
      detail::to_bytes_router<O>(cast_value<bool>(value, "bool", field), result,
                                 byte_index);
      break;
    case 'c':
      detail::to_bytes_router<O>(cast_value<char>(value, "char", field), result,
                                 byte_index);
      break;
    case 'b':
      detail::to_bytes_router<O>(cast_value<int8_t>(value, "int8", field),
                                 result, byte_index);
      break;
    case 'B':
      detail::to_bytes_router<O>(cast_value<uint8_t>(value, "uint8", field),
                                 result, byte_index);
      break;
    case 'h':
      detail::to_bytes_router<O>(cast_value<int16_t>(value, "int16", field),
                                 result, byte_index);
      break;
    case 'H':
      detail::to_bytes_router<O>(cast_value<uint16_t>(value, "uint16", field),
                                 result, byte_index);
      break;
    case 'i':
      detail::to_bytes_router<O>(cast_value<int32_t>(value, "int32", field),
                                 result, byte_index);
      break;
    case 'I':
      detail::to_bytes_router<O>(cast_value<uint32_t>(value, "uint32", field),
                                 result, byte_index);
      break;
    case 'q':
      detail::to_bytes_router<O>(cast_value<int64_t>(value, "int64", field),
                                 result, byte_index);
      break;
    case 'Q':
      detail::to_bytes_router<O>(cast_value<uint64_t>(value, "uint64", field),
                                 result, byte_index);
      break;
    case 'f':
      detail::to_bytes_router<O>(cast_value<float>(value, "float", field),
                                 result, byte_index);
      break;
    case 'd':
      detail::to_bytes_router<O>(cast_value<double>(value, "double", field),
                                 result, byte_index);
      break;
    case 'N':
      detail::to_bytes_router<O>(
          cast_value<std::size_t>(value, "size_t", field), result, byte_index);
      break;
    case 's':
      detail::to_bytes_router<O>(cast_value<std::string>(value, "str", field),
                                 result, byte_index);
      break;
    case '[':
      // either a vector or an array
      // if array, next character is a digit - size of the array
      if (index + 1 < format.size() &&
          std::isdigit(static_cast<unsigned char>(format[index + 1]))) {
        parse_array<O>(format, value, index, result, byte_index, field);
      } else {
        parse_vector<O>(format, value, index, result, byte_index);
      }
      break;
    case '{': {
      // either a dictionary ("{K:V}") or a set ("{T}")
      auto value_type = get_value_type_map(format, index);
      std::size_t colon = 0;
      if (is_map_spec(value_type, colon)) {
        parse_unordered_map<O>(value_type, colon, value, result, byte_index);
      } else {
        parse_unordered_set<O>(value_type, value, result, byte_index);
      }
      break;
    }
    case '(': {
      auto value_type = get_value_type_tuple(format, index);
      parse_tuple<O>(value_type, value, result, byte_index);
      break;
    }
    default:
      throw format_error(std::string("unknown format code '") + code +
                         "' at index " + std::to_string(index));
    }
    index += 1;
  }

  if (index != format.size()) {
    throw format_error("format '" + format +
                       "' describes more fields than the " +
                       std::to_string(py::len(args)) + " values given");
  }

  return result;
}

} // namespace python

} // namespace alpaca
