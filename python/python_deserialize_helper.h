#pragma once
#include "python_serialize_helper.h"

namespace py = pybind11;

namespace alpaca {

namespace python {

py::list deserialize(const std::string &format,
                     const std::vector<uint8_t> &bytes,
                     std::size_t &byte_index);

template <options OPTIONS, typename T>
void load_simple_type(const std::vector<uint8_t> &bytes,
                      std::size_t &byte_index, std::size_t &end_index,
                      std::error_code &error_code, py::list &result,
                      const std::string &type_name) {
  T output{};
  detail::from_bytes_router<OPTIONS>(output, bytes, byte_index, end_index,
                                     error_code);
  if (error_code) {
    throw std::runtime_error("Error parsing " + type_name);
  }
  result.append(output);
}

void load_array(const std::string &format, std::size_t &index,
                const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                py::list &result) {
  // get past '['
  index++;

  auto size = get_size(format, index);

  // No need to deserialize the size

  auto value_type = get_value_type_array(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  // recurse - load all values
  result.append(deserialize(value_format_list, bytes, byte_index));
}

template <options OPTIONS>
void load_vector(const std::string &format, std::size_t &index,
                 const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                 std::size_t &end_index, py::list &result) {
  // field is a std::vector

  // load size
  std::error_code error_code;
  std::size_t size = 0;
  detail::from_bytes<OPTIONS, std::size_t>(size, bytes, byte_index, end_index,
                                           error_code);

  if (size > end_index - byte_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    throw std::runtime_error("Invalid vector size");
  }

  auto value_type = get_value_type_vector(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  // recurse - load all values
  result.append(deserialize(value_format_list, bytes, byte_index));
}

template <options OPTIONS>
void load_unordered_map(const std::string &key_value_type,
                        const std::vector<uint8_t> &bytes,
                        std::size_t &byte_index, std::size_t &end_index,
                        py::list &result) {
  // field is a std::unordered_map

  // load size
  std::error_code error_code;
  std::size_t size = 0;
  detail::from_bytes<OPTIONS, std::size_t>(size, bytes, byte_index, end_index,
                                           error_code);

  if (size > end_index - byte_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    throw std::runtime_error("Invalid map size");
  }

  auto keyit = key_value_type.find(':'); // every until ':' is the key
  auto key_type = key_value_type.substr(0, keyit);
  auto value_type = key_value_type.substr(keyit + 1);

  py::dict dict;
  for (std::size_t i = 0; i < size; ++i) {
    auto key = deserialize(key_type, bytes, byte_index);
    auto value = deserialize(value_type, bytes, byte_index);
    dict[key[0]] = value[0];
  }

  result.append(dict);
}

template <options OPTIONS>
void load_unordered_set(const std::string &value_type,
                        const std::vector<uint8_t> &bytes,
                        std::size_t &byte_index, std::size_t &end_index,
                        py::list &result) {
  // field is a std::unordered_set

  // load size
  std::error_code error_code;
  std::size_t size = 0;
  detail::from_bytes<OPTIONS, std::size_t>(size, bytes, byte_index, end_index,
                                           error_code);

  if (size > end_index - byte_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    throw std::runtime_error("Invalid set size");
  }

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  // recurse - load all values
  result.append(
      py::cast<py::set>(deserialize(value_format_list, bytes, byte_index)));
}

template <options OPTIONS>
void load_tuple(const std::string &format, std::size_t &index,
                const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                py::list &result) {

  auto value_type = get_value_type_tuple(format, index);

  result.append(
      py::cast<py::tuple>(deserialize(value_type, bytes, byte_index)));
}

py::list deserialize(const std::string &format,
                     const std::vector<uint8_t> &bytes,
                     std::size_t &byte_index) {
  py::list result;

  std::error_code error_code;
  std::size_t end_index = bytes.size();

  constexpr auto OPTIONS = alpaca::options::none;

  std::size_t index = 0;

  while (byte_index < end_index && index < format.size()) {
    if (format[index] == '?') {
      // field is a bool
      load_simple_type<OPTIONS, bool>(bytes, byte_index, end_index, error_code,
                                      result, "bool");
    } else if (format[index] == 'c') {
      // field is a character
      load_simple_type<OPTIONS, char>(bytes, byte_index, end_index, error_code,
                                      result, "char");
    } else if (format[index] == 'b') {
      // field is a int8_t
      load_simple_type<OPTIONS, int8_t>(bytes, byte_index, end_index,
                                        error_code, result, "int8_t");
    } else if (format[index] == 'B') {
      // field is a uint8_t
      load_simple_type<OPTIONS, uint8_t>(bytes, byte_index, end_index,
                                         error_code, result, "uint8_t");
    } else if (format[index] == 'h') {
      // field is a int16_t
      load_simple_type<OPTIONS, int16_t>(bytes, byte_index, end_index,
                                         error_code, result, "int16_t");
    } else if (format[index] == 'H') {
      // field is a uint16_t
      load_simple_type<OPTIONS, uint16_t>(bytes, byte_index, end_index,
                                          error_code, result, "uint16_t");
    } else if (format[index] == 'i') {
      // field is a int32_t
      load_simple_type<OPTIONS, int32_t>(bytes, byte_index, end_index,
                                         error_code, result, "int32_t");
    } else if (format[index] == 'I') {
      // field is a uint32_t
      load_simple_type<OPTIONS, uint32_t>(bytes, byte_index, end_index,
                                          error_code, result, "uint32_t");
    } else if (format[index] == 'q') {
      // field is a int64_t
      load_simple_type<OPTIONS, int64_t>(bytes, byte_index, end_index,
                                         error_code, result, "int64_t");
    } else if (format[index] == 'Q') {
      // field is a uint64_t
      load_simple_type<OPTIONS, uint64_t>(bytes, byte_index, end_index,
                                          error_code, result, "uint64_t");
    } else if (format[index] == 'f') {
      // field is a float
      load_simple_type<OPTIONS, float>(bytes, byte_index, end_index, error_code,
                                       result, "float");
    } else if (format[index] == 'd') {
      // field is a double
      load_simple_type<OPTIONS, double>(bytes, byte_index, end_index,
                                        error_code, result, "double");
    } else if (format[index] == 'N') {
      // field is a std::size_t
      load_simple_type<OPTIONS, std::size_t>(bytes, byte_index, end_index,
                                             error_code, result, "std::size_t");
    } else if (format[index] == 's') {
      // field is a std::string
      load_simple_type<OPTIONS, std::string>(bytes, byte_index, end_index,
                                             error_code, result, "std::string");
    } else if (format[index] == '[') {
      // either a vector or an array
      // if array, next character is a digit - size of the array
      if (std::isdigit(format[index + 1])) {
        load_array(format, index, bytes, byte_index, result);
      } else {
        load_vector<OPTIONS>(format, index, bytes, byte_index, end_index,
                             result);
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
            load_unordered_map<OPTIONS>(value_type, bytes, byte_index,
                                        end_index, result);
          } else {
            // set
            load_unordered_set<OPTIONS>(value_type, bytes, byte_index,
                                        end_index, result);
          }
        } else {
          // dictionary
          load_unordered_map<OPTIONS>(value_type, bytes, byte_index, end_index,
                                      result);
        }
      } else {
        // set
        load_unordered_set<OPTIONS>(value_type, bytes, byte_index, end_index,
                                    result);
      }
    } else if (format[index] == '(') {
      // tuple
      load_tuple<OPTIONS>(format, index, bytes, byte_index, result);
    }

    index += 1;
  }

  return result;
}

py::list do_deserialize(const std::string &format, const py::bytes &bytes) {
  std::vector<uint8_t> bytes_vector{};
  std::size_t byte_index = 0;
  for (const auto &b : bytes) {
    bytes_vector.push_back(py::cast<uint8_t>(b));
  }
  return deserialize(format, bytes_vector, byte_index);
}

} // namespace python

} // namespace alpaca