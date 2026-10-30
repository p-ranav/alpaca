#include "python_deserialize_helper.h"
#include "python_serialize_helper.h"

namespace alpaca {
namespace python {

// Options that change how the payload bytes are written / read, and therefore
// need their own template instantiation of the helpers. with_checksum is
// handled at the top level (a CRC32 around the payload); force_aligned_access
// does not change the bytes; with_version is rejected (see below).
constexpr unsigned ENCODING_MASK =
    static_cast<unsigned>(options::big_endian) |
    static_cast<unsigned>(options::fixed_length_encoding);
constexpr unsigned DECODING_MASK =
    ENCODING_MASK | static_cast<unsigned>(options::strict);
constexpr unsigned KNOWN_MASK =
    DECODING_MASK | static_cast<unsigned>(options::with_checksum) |
    static_cast<unsigned>(options::with_version) |
    static_cast<unsigned>(options::force_aligned_access);

inline void check_options(unsigned opts) {
  if (opts & ~KNOWN_MASK) {
    throw format_error("unknown option bits " +
                       std::to_string(opts & ~KNOWN_MASK));
  }
  if (opts & static_cast<unsigned>(options::with_version)) {
    throw format_error("options.with_version is not supported from a format "
                       "string: alpaca's version hash is "
                       "built from the C++ type (it includes sizeof(struct) "
                       "and the field count), which a "
                       "format string cannot reproduce");
  }
}

inline bool has(unsigned opts, options o) {
  return (opts & static_cast<unsigned>(o)) != 0;
}

// Call f.template operator()<O>() with O = the encoding-relevant bits of opts.
template <typename F> auto with_encoding_options(unsigned opts, F &&f) {
  constexpr auto BE = options::big_endian;
  constexpr auto FL = options::fixed_length_encoding;
  constexpr auto ST = options::strict;
  switch (opts & DECODING_MASK) {
  case 0:
    return f.template operator()<options::none>();
  case static_cast<unsigned>(BE):
    return f.template operator()<BE>();
  case static_cast<unsigned>(FL):
    return f.template operator()<FL>();
  case static_cast<unsigned>(BE) | static_cast<unsigned>(FL):
    return f.template operator()<BE | FL>();
  case static_cast<unsigned>(ST):
    return f.template operator()<ST>();
  case static_cast<unsigned>(BE) | static_cast<unsigned>(ST):
    return f.template operator()<BE | ST>();
  case static_cast<unsigned>(FL) | static_cast<unsigned>(ST):
    return f.template operator()<FL | ST>();
  default:
    return f.template operator()<BE | FL | ST>();
  }
}

py::bytes do_serialize(const std::string &format, const py::list &args,
                       unsigned opts) {
  check_options(opts);
  // strict does not change how bytes are written
  const unsigned encoding = opts & ENCODING_MASK;
  auto bytes = with_encoding_options(
      encoding, [&]<options O>() { return serialize<O>(format, args); });
  if (has(opts, options::with_checksum)) {
    // calculate crc32 for byte array and pack uint32_t to the end, as alpaca
    // does
    std::size_t byte_index = bytes.size();
    uint32_t crc = crc32_fast(bytes.data(), bytes.size());
    if (has(opts, options::big_endian)) {
      detail::to_bytes_crc32<options::big_endian>(bytes, byte_index, crc);
    } else {
      detail::to_bytes_crc32<options::none>(bytes, byte_index, crc);
    }
  }
  return py::bytes(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

py::list do_deserialize(const std::string &format, const py::bytes &input,
                        unsigned opts) {
  check_options(opts);
  const std::string s = input;
  std::vector<uint8_t> bytes(s.begin(), s.end());
  std::size_t end_index = bytes.size();
  if (has(opts, options::with_checksum)) {
    if (end_index < 4) {
      throw decode_error("input of " + std::to_string(end_index) +
                         " bytes is too short to carry a checksum");
    }
    uint32_t trailing_crc = 0;
    std::size_t index = end_index - 4;
    std::error_code ec;
    if (has(opts, options::big_endian)) {
      detail::from_bytes_crc32<options::big_endian>(trailing_crc, bytes, index,
                                                    end_index, ec);
    } else {
      detail::from_bytes_crc32<options::none>(trailing_crc, bytes, index,
                                              end_index, ec);
    }
    const uint32_t computed_crc = crc32_fast(bytes.data(), end_index - 4);
    if (trailing_crc != computed_crc) {
      throw decode_error("checksum mismatch");
    }
    end_index -= 4;
  }
  std::size_t byte_index = 0;
  return with_encoding_options(opts, [&]<options O>() {
    return deserialize<O>(format, bytes, byte_index, end_index);
  });
}

} // namespace python
} // namespace alpaca

PYBIND11_MODULE(pyalpaca, m) {
  using alpaca::options;
  m.doc() = "Python wrapper for alpaca: serialize / deserialize values "
            "described by a format "
            "string (see the alpaca README, 'Python Interoperability')";

  py::enum_<options>(
      m, "options", py::arithmetic(),
      "alpaca::options bits; combine with |, pass as the `options` argument")
      .value("none", options::none)
      .value("big_endian", options::big_endian)
      .value("fixed_length_encoding", options::fixed_length_encoding)
      .value("with_version", options::with_version)
      .value("with_checksum", options::with_checksum)
      .value("force_aligned_access", options::force_aligned_access)
      .value("strict", options::strict);

  m.def("serialize", &alpaca::python::do_serialize, py::arg("format"),
        py::arg("values"), py::arg("options") = 0,
        "Serialize `values` (a list, one entry per field of `format`) to "
        "bytes.\n\n"
        "`options` is an int or a combination of pyalpaca.options bits and "
        "must match the\n"
        "options the C++ side uses: big_endian, fixed_length_encoding and "
        "with_checksum\n"
        "change the bytes; with_version raises ValueError (its hash cannot be "
        "computed from\n"
        "a format string). ValueError for a bad format or mismatched values.");
  m.def("deserialize", &alpaca::python::do_deserialize, py::arg("format"),
        py::arg("data"), py::arg("options") = 0,
        "Deserialize `data` (bytes) into a list, one entry per field of "
        "`format`.\n\n"
        "With `strict`, input that ends before the format does raises "
        "RuntimeError;\n"
        "without it the missing trailing fields are simply not returned "
        "(alpaca's\n"
        "forward-compatibility rule). with_checksum verifies and strips the "
        "trailing CRC32.\n"
        "RuntimeError for truncated input or a bad checksum, ValueError for a "
        "bad format.");
}
