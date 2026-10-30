#pragma once
#include "python_serialize_helper.h"

namespace py = pybind11;

namespace alpaca {

namespace python {

template <options O>
py::list deserialize(const std::string &format,
                     const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                     std::size_t end_index);

template <options O, typename T>
void load_simple_type(const std::vector<uint8_t> &bytes,
                      std::size_t &byte_index, std::size_t &end_index,
                      py::list &result, const char *type_name) {
  T output{};
  std::error_code error_code;
  detail::from_bytes_router<O>(output, bytes, byte_index, end_index,
                               error_code);
  if (error_code) {
    throw decode_error(std::string("error reading ") + type_name + " at byte " +
                       std::to_string(byte_index) + ": " +
                       error_code.message());
  }
  result.append(output);
}

// Read a container size the way alpaca stores it (size_t_serialized_type) and
// check it against the bytes remaining.
template <options O>
std::size_t load_size(const std::vector<uint8_t> &bytes,
                      std::size_t &byte_index, std::size_t &end_index,
                      const char *what) {
  std::error_code error_code;
  detail::size_t_serialized_type size = 0;
  detail::from_bytes<O, detail::size_t_serialized_type>(size, bytes, byte_index,
                                                        end_index, error_code);
  if (error_code) {
    throw decode_error(std::string("error reading ") + what + " size at byte " +
                       std::to_string(byte_index) + ": " +
                       error_code.message());
  }
  if (size > end_index - byte_index) {
    // size is greater than the number of bytes remaining
    throw decode_error(std::string("invalid ") + what + " size " +
                       std::to_string(size) + " at byte " +
                       std::to_string(byte_index));
  }
  return size;
}

template <options O>
void load_array(const std::string &format, std::size_t &index,
                const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                std::size_t end_index, py::list &result) {
  // get past '['
  index++;

  auto size = get_size(format, index);

  // No need to deserialize the size
  auto value_type = get_value_type_array(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  result.append(
      deserialize<O>(value_format_list, bytes, byte_index, end_index));
}

template <options O>
void load_vector(const std::string &format, std::size_t &index,
                 const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                 std::size_t end_index, py::list &result) {
  auto size = load_size<O>(bytes, byte_index, end_index, "vector");

  auto value_type = get_value_type_vector(format, index);

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  result.append(
      deserialize<O>(value_format_list, bytes, byte_index, end_index));
}

template <options O>
void load_unordered_map(const std::string &key_value_type, std::size_t colon,
                        const std::vector<uint8_t> &bytes,
                        std::size_t &byte_index, std::size_t end_index,
                        py::list &result) {
  auto size = load_size<O>(bytes, byte_index, end_index, "map");

  auto key_type = key_value_type.substr(0, colon);
  auto value_type = key_value_type.substr(colon + 1);

  py::dict dict;
  for (std::size_t i = 0; i < size; ++i) {
    auto key = deserialize<O>(key_type, bytes, byte_index, end_index);
    auto value = deserialize<O>(value_type, bytes, byte_index, end_index);
    dict[key[0]] = value[0];
  }

  result.append(dict);
}

template <options O>
void load_unordered_set(const std::string &value_type,
                        const std::vector<uint8_t> &bytes,
                        std::size_t &byte_index, std::size_t end_index,
                        py::list &result) {
  auto size = load_size<O>(bytes, byte_index, end_index, "set");

  std::string value_format_list{""};
  for (std::size_t i = 0; i < size; ++i) {
    value_format_list += value_type;
  }

  result.append(py::cast<py::set>(
      deserialize<O>(value_format_list, bytes, byte_index, end_index)));
}

template <options O>
void load_tuple(const std::string &format, std::size_t &index,
                const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                std::size_t end_index, py::list &result) {
  auto value_type = get_value_type_tuple(format, index);
  result.append(py::cast<py::tuple>(
      deserialize<O>(value_type, bytes, byte_index, end_index)));
}

template <options O>
py::list deserialize(const std::string &format,
                     const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                     std::size_t end_index) {
  py::list result;
  std::size_t index = 0;

  while (index < format.size()) {
    if (byte_index >= end_index) {
      // input ended before the format did
      if constexpr (detail::strict<O>()) {
        throw decode_error("input ended after " + std::to_string(byte_index) +
                           " bytes, before field " +
                           std::to_string(py::len(result)) + " of format '" +
                           format + "'");
      } else {
        // forward compatibility: the remaining fields are simply not returned
        break;
      }
    }
    const char code = format[index];
    switch (code) {
    case '?':
      load_simple_type<O, bool>(bytes, byte_index, end_index, result, "bool");
      break;
    case 'c':
      load_simple_type<O, char>(bytes, byte_index, end_index, result, "char");
      break;
    case 'b':
      load_simple_type<O, int8_t>(bytes, byte_index, end_index, result, "int8");
      break;
    case 'B':
      load_simple_type<O, uint8_t>(bytes, byte_index, end_index, result,
                                   "uint8");
      break;
    case 'h':
      load_simple_type<O, int16_t>(bytes, byte_index, end_index, result,
                                   "int16");
      break;
    case 'H':
      load_simple_type<O, uint16_t>(bytes, byte_index, end_index, result,
                                    "uint16");
      break;
    case 'i':
      load_simple_type<O, int32_t>(bytes, byte_index, end_index, result,
                                   "int32");
      break;
    case 'I':
      load_simple_type<O, uint32_t>(bytes, byte_index, end_index, result,
                                    "uint32");
      break;
    case 'q':
      load_simple_type<O, int64_t>(bytes, byte_index, end_index, result,
                                   "int64");
      break;
    case 'Q':
      load_simple_type<O, uint64_t>(bytes, byte_index, end_index, result,
                                    "uint64");
      break;
    case 'f':
      load_simple_type<O, float>(bytes, byte_index, end_index, result, "float");
      break;
    case 'd':
      load_simple_type<O, double>(bytes, byte_index, end_index, result,
                                  "double");
      break;
    case 'N':
      load_simple_type<O, std::size_t>(bytes, byte_index, end_index, result,
                                       "size_t");
      break;
    case 's':
      load_simple_type<O, std::string>(bytes, byte_index, end_index, result,
                                       "str");
      break;
    case '[':
      // either a vector or an array
      // if array, next character is a digit - size of the array
      if (index + 1 < format.size() &&
          std::isdigit(static_cast<unsigned char>(format[index + 1]))) {
        load_array<O>(format, index, bytes, byte_index, end_index, result);
      } else {
        load_vector<O>(format, index, bytes, byte_index, end_index, result);
      }
      break;
    case '{': {
      auto value_type = get_value_type_map(format, index);
      std::size_t colon = 0;
      if (is_map_spec(value_type, colon)) {
        load_unordered_map<O>(value_type, colon, bytes, byte_index, end_index,
                              result);
      } else {
        load_unordered_set<O>(value_type, bytes, byte_index, end_index, result);
      }
      break;
    }
    case '(':
      load_tuple<O>(format, index, bytes, byte_index, end_index, result);
      break;
    default:
      throw format_error(std::string("unknown format code '") + code +
                         "' at index " + std::to_string(index));
    }
    index += 1;
  }

  return result;
}

} // namespace python

} // namespace alpaca
