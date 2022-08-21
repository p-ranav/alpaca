#pragma once
#include <cstdint>

namespace detail {

enum class type : uint8_t {
  boolean,
  uint8,
  uint16_as_uint8,
  uint16,
  uint32_as_uint8,
  uint32_as_uint16,
  uint32,
  uint64_as_uint8,
  uint64_as_uint16,
  uint64_as_uint32,
  uint64,
  int8,
  int16_as_int8,
  int16,
  int32_as_int8,
  int32_as_int16,
  int32,
  int64_as_int8,
  int64_as_int16,
  int64_as_int32,
  int64,
  float32,
  float64,
  string,
  vector,
  tuple,
  pair,
  struct_
};

}