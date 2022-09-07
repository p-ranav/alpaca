#pragma once
#include <alpaca/alpaca.h>
#include <cctype>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

namespace alpaca {

namespace python {

std::vector<uint8_t> serialize(const std::string &format, const py::list &args);

std::size_t get_size(const std::string &format, std::size_t &index) {
  std::string chars{""};

  // current is an digit
  auto current = [&]() { return format[index]; };
  auto move_one = [&]() { index++; };

  while (index > format.size() || std::isdigit(current())) {
    chars += current();
    move_one();
  }

  return std::stoll(chars);
}

auto get_value_type_array(const std::string &format, std::size_t &index) {
  std::string result{};

  auto current = [&]() { return format[index]; };
  auto move_one = [&]() { index++; };

  // current should not be ']'
  if (current() == ']') {
    throw std::runtime_error("Expected type, instead got ']'");
  }

  std::size_t level = 1;
  while (index < format.size()) {
    // base case
    if (current() == ']') {
      if (level > 0) {
        level -= 1;
      }
      if (level == 0) {
        break;
      } else {
        result += current();
        move_one();
      }
    } else {
      if (current() == '[') {
        level += 1;
      }
      result += current();
      move_one();
    }
  }

  return result;
}

template <options OPTIONS, class It>
void parse_array(const std::string &format, It it, std::size_t &index,
                 std::vector<uint8_t> &result) {
  // get past '['
  index++;

  const std::size_t input_size = py::len(py::cast<py::list>(*it));
  auto size = get_size(format, index);

  if (input_size != size) {
    throw std::runtime_error("Expected " + std::to_string(size) +
                             " values, instead found " +
                             std::to_string(input_size));
  }

  // No need to serialize the size

  auto value_type = get_value_type_array(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  // recurse - save all values
  auto list = py::cast<py::list>(*it);
  auto serialized = serialize(value_format_list, list);

  for (auto &b : serialized) {
    result.push_back(std::move(b));
  }
}

auto get_value_type_vector(const std::string &format, std::size_t &index) {
  std::string result{};

  auto current = [&]() { return format[index]; };
  auto move_one = [&]() { index++; };

  // get past '['
  move_one();

  // current should not be ']'
  if (current() == ']') {
    throw std::runtime_error("Expected type, instead got ']'");
  }

  std::size_t level = 1;
  while (index < format.size()) {
    // base case
    if (current() == ']') {
      if (level > 0) {
        level -= 1;
      }
      if (level == 0) {
        break;
      } else {
        result += current();
        move_one();
      }
    } else {
      if (current() == '[') {
        level += 1;
      }
      result += current();
      move_one();
    }
  }

  return result;
}

template <options OPTIONS, class It>
void parse_vector(const std::string &format, It it, std::size_t &index,
                  std::vector<uint8_t> &result, std::size_t &byte_index) {
  // field is a std::vector

  // serialize size
  const auto size = py::len(py::cast<py::list>(*it));
  detail::to_bytes_router<OPTIONS>(size, result, byte_index);

  auto value_type = get_value_type_vector(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  // recurse - save all values
  auto list = py::cast<py::list>(*it);
  auto serialized = serialize(value_format_list, list);

  for (auto &b : serialized) {
    result.push_back(std::move(b));
  }
}

auto get_value_type_map(const std::string &format, std::size_t &index) {
  std::string result{};

  auto current = [&]() { return format[index]; };
  auto move_one = [&]() { index++; };

  // get past '{'
  move_one();

  // current should not be '}'
  if (current() == '}') {
    throw std::runtime_error("Expected type, instead got '}'");
  }

  std::size_t level = 1;
  while (index < format.size()) {
    // base case
    if (current() == '}') {
      if (level > 0) {
        level -= 1;
      }
      if (level == 0) {
        break;
      } else {
        result += current();
        move_one();
      }
    } else {
      if (current() == '{') {
        level += 1;
      }
      result += current();
      move_one();
    }
  }

  return result;
}

template <options OPTIONS, class It>
void parse_unordered_map(const std::string &key_value_type, It it,
                         std::vector<uint8_t> &result,
                         std::size_t &byte_index) {
  // field is a std::unordered_map

  // serialize size
  const auto size = py::len(py::cast<py::dict>(*it));
  detail::to_bytes_router<OPTIONS>(size, result, byte_index);

  auto keyit = key_value_type.find(':'); // every until ':' is the key
  auto key_type = key_value_type.substr(0, keyit);
  auto value_type = key_value_type.substr(keyit + 1);

  // recurse - save all values
  auto dict = py::cast<py::dict>(*it);
  for (auto item : dict) {
    py::list keys;
    keys.append(item.first);
    auto key_serialized = serialize(key_type, keys);

    for (auto &b : key_serialized) {
      result.push_back(std::move(b));
    }

    py::list values;
    values.append(item.second);
    auto value_serialized = serialize(value_type, values);

    for (auto &b : value_serialized) {
      result.push_back(std::move(b));
    }
  }
}

template <options OPTIONS, class It>
void parse_unordered_set(const std::string &value_type, It it,
                         std::vector<uint8_t> &result,
                         std::size_t &byte_index) {
  // field is a std::unordered_set

  // serialize size
  const auto size = py::len(py::cast<py::set>(*it));
  detail::to_bytes_router<OPTIONS>(size, result, byte_index);

  // recurse - save all values
  auto set = py::cast<py::set>(*it);
  for (auto item : set) {
    py::list items;
    items.append(item);
    auto serialized = serialize(value_type, items);

    for (auto &b : serialized) {
      result.push_back(std::move(b));
    }
  }
}

auto get_value_type_tuple(const std::string &format, std::size_t &index) {
  std::string result{};

  auto current = [&]() { return format[index]; };
  auto move_one = [&]() { index++; };

  // get past '('
  move_one();

  // current should not be ')'
  if (current() == ')') {
    throw std::runtime_error("Expected type, instead got ')'");
  }

  std::size_t level = 1;
  while (index < format.size()) {
    // base case
    if (current() == ')') {
      if (level > 0) {
        level -= 1;
      }
      if (level == 0) {
        break;
      } else {
        result += current();
        move_one();
      }
    } else {
      if (current() == '(') {
        level += 1;
      }
      result += current();
      move_one();
    }
  }

  return result;
}

template <options OPTIONS, class It>
void parse_tuple(const std::string &value_type, It it,
                 std::vector<uint8_t> &result) {
  const auto tuple = py::cast<py::tuple>(*it);

  auto list = py::cast<py::list>(tuple);
  auto serialized = serialize(value_type, list);

  for (auto &b : serialized) {
    result.push_back(std::move(b));
  }
}

std::vector<uint8_t> serialize(const std::string &format,
                               const py::list &args) {
  std::vector<uint8_t> result;
  std::size_t byte_index = 0;

  constexpr auto OPTIONS = alpaca::options::none;

  std::size_t index = 0;

  for (auto it = args.begin(); it != args.end(); ++it) {
    if (format[index] == '?') {
      // field is a bool
      detail::to_bytes_router<OPTIONS>(it->cast<bool>(), result, byte_index);
    } else if (format[index] == 'c') {
      // field is a character
      detail::to_bytes_router<OPTIONS>(it->cast<char>(), result, byte_index);
    } else if (format[index] == 'b') {
      // field is a int8_t
      detail::to_bytes_router<OPTIONS>(it->cast<int8_t>(), result, byte_index);
    } else if (format[index] == 'B') {
      // field is a uint8_t
      detail::to_bytes_router<OPTIONS>(it->cast<uint8_t>(), result, byte_index);
    } else if (format[index] == 'h') {
      // field is a int16_t
      detail::to_bytes_router<OPTIONS>(it->cast<int16_t>(), result, byte_index);
    } else if (format[index] == 'H') {
      // field is a uint16_t
      detail::to_bytes_router<OPTIONS>(it->cast<uint16_t>(), result,
                                       byte_index);
    } else if (format[index] == 'i') {
      // field is a int32_t
      detail::to_bytes_router<OPTIONS>(it->cast<int32_t>(), result, byte_index);
    } else if (format[index] == 'I') {
      // field is a uint32_t
      detail::to_bytes_router<OPTIONS>(it->cast<uint32_t>(), result,
                                       byte_index);
    } else if (format[index] == 'q') {
      // field is a int64_t
      detail::to_bytes_router<OPTIONS>(it->cast<int64_t>(), result, byte_index);
    } else if (format[index] == 'Q') {
      // field is a uint64_t
      detail::to_bytes_router<OPTIONS>(it->cast<uint64_t>(), result,
                                       byte_index);
    } else if (format[index] == 'f') {
      // field is a float
      detail::to_bytes_router<OPTIONS>(it->cast<float>(), result, byte_index);
    } else if (format[index] == 'd') {
      // field is a double
      detail::to_bytes_router<OPTIONS>(it->cast<double>(), result, byte_index);
    } else if (format[index] == 'N') {
      // field is a std::size_t
      detail::to_bytes_router<OPTIONS>(it->cast<std::size_t>(), result,
                                       byte_index);
    } else if (format[index] == 's') {
      // field is a std::string
      detail::to_bytes_router<OPTIONS>(it->cast<std::string>(), result,
                                       byte_index);
    } else if (format[index] == '[') {

      // either a vector or an array
      // if array, next character is a digit - size of the array
      if (std::isdigit(format[index + 1])) {
        parse_array<OPTIONS>(format, it, index, result);
      } else {
        parse_vector<OPTIONS>(format, it, index, result, byte_index);
      }
    } else if (format[index] == '{') {
      // either a dictionary or a set
      // if dictionary, has ':' between key and value
      auto value_type = get_value_type_map(format, index);

      auto curly = value_type.find('{');
      auto colon = value_type.find(':');
      if (colon != std::string::npos) {
        // could be a set of dictionaries (unlikely)
        if (curly != std::string::npos) {
          if (colon < curly) {
            // dictionary
            parse_unordered_map<OPTIONS>(value_type, it, result, byte_index);
          } else {
            // set
            parse_unordered_set<OPTIONS>(value_type, it, result, byte_index);
          }
        } else {
          // dictionary
          parse_unordered_map<OPTIONS>(value_type, it, result, byte_index);
        }
      } else {
        // set
        parse_unordered_set<OPTIONS>(value_type, it, result, byte_index);
      }
    } else if (format[index] == '(') {
      // tuple
      auto value_type = get_value_type_tuple(format, index);

      parse_tuple<OPTIONS>(value_type, it, result);
    }

    index += 1;
  }

  return result;
}

py::list do_serialize_list(const std::string &format, const py::list &args) {
  auto bytes = serialize(format, args);
  py::list pyresult = py::cast(bytes);
  return pyresult;
}

py::bytes do_serialize(const std::string &format, const py::list &args) {
  auto bytes = serialize(format, args);
  std::string bytestring{""};
  for (const auto &b : bytes) {
    bytestring += static_cast<char>(b);
  }
  return py::bytes(bytestring);
}

} // namespace python

} // namespace alpaca