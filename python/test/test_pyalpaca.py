"""Tests for the pyalpaca module.

Run from the repo root after building with -DALPACA_BUILD_PYTHON_LIB=on:

    PYTHONPATH=build/python python3 -m unittest discover -s python/test -v

vectors.txt holds the bytes the C++ library produces for one set of values
under each supported option combination (see generate_vectors.cpp), so the
cross-language checks do not depend on a C++ toolchain at test time.
"""

import os
import unittest

import pyalpaca

HERE = os.path.dirname(os.path.abspath(__file__))

EVERYTHING_FORMAT = "?cbBhHiIqQfds[i][3B]{s:i}(cif)"
EVERYTHING_VALUE = [
    True,
    "x",
    -5,
    200,
    -1234,
    54321,
    -100000,
    3000000000,
    -5000000000,
    10000000000,
    1.5,
    -2.25,
    "hello",
    [1, -2, 300],
    [7, 8, 9],
    {"a": 1, "b": 2},
    ("z", 42, 0.5),
]

OPTIONS_BY_NAME = {
    "none": 0,
    "big_endian": pyalpaca.options.big_endian,
    "fixed_length_encoding": pyalpaca.options.fixed_length_encoding,
    "with_checksum": pyalpaca.options.with_checksum,
    "strict": pyalpaca.options.strict,
}


def parse_options(name):
    value = 0
    for part in name.split("|"):
        value |= int(OPTIONS_BY_NAME[part])
    return value


def load_vectors():
    vectors = {}
    with open(os.path.join(HERE, "vectors.txt")) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            name, hexbytes = line.split()
            vectors[name] = bytes.fromhex(hexbytes)
    return vectors


class RoundTrip(unittest.TestCase):
    def check_round_trip(self, fmt, value, options=0):
        data = pyalpaca.serialize(fmt, value, options)
        self.assertIsInstance(data, bytes)
        self.assertEqual(pyalpaca.deserialize(fmt, data, options), value)
        return data

    def test_scalars(self):
        self.check_round_trip("?cbBhHiIqQfd", [False, "q", -1, 255, -2, 65535, -3, 4, -5, 6, 0.25, -1e9])

    def test_string_and_containers(self):
        self.check_round_trip("s[s][2i]{i:s}{B}(sf)", ["abc", ["d", "", "ef"], [1, 2], {1: "one", 2: "two"}, {1, 2, 3}, ("t", 0.5)])

    def test_nested(self):
        self.check_round_trip("[[i]][2[s]]{s:[i]}([i](cs))", [[[1], [], [2, 3]], [["a"], ["b", "c"]], {"k": [4, 5]}, ([6], ("d", "e"))])

    def test_everything_each_option(self):
        for name in ("none", "big_endian", "fixed_length_encoding", "big_endian|fixed_length_encoding", "with_checksum", "fixed_length_encoding|with_checksum|strict", "big_endian|fixed_length_encoding|with_checksum|strict"):
            with self.subTest(options=name):
                self.check_round_trip(EVERYTHING_FORMAT, EVERYTHING_VALUE, parse_options(name))

    def test_options_enum_and_int_are_interchangeable(self):
        a = pyalpaca.serialize("i", [7], pyalpaca.options.fixed_length_encoding)
        b = pyalpaca.serialize("i", [7], 2)
        self.assertEqual(a, b)
        self.assertEqual(len(a), 4)
        self.assertEqual(len(pyalpaca.serialize("i", [7])), 1)  # variable-length by default

    def test_empty(self):
        self.assertEqual(pyalpaca.serialize("", []), b"")
        self.assertEqual(pyalpaca.deserialize("", b""), [])


class CppVectors(unittest.TestCase):
    """pyalpaca must produce and consume exactly what alpaca::serialize<O>() does."""

    @classmethod
    def setUpClass(cls):
        cls.vectors = load_vectors()
        if not cls.vectors:
            raise unittest.SkipTest("vectors.txt is empty")

    def test_serialize_matches_cpp(self):
        for name, expected in self.vectors.items():
            with self.subTest(options=name):
                self.assertEqual(pyalpaca.serialize(EVERYTHING_FORMAT, EVERYTHING_VALUE, parse_options(name)), expected)

    def test_deserialize_reads_cpp(self):
        for name, data in self.vectors.items():
            with self.subTest(options=name):
                self.assertEqual(pyalpaca.deserialize(EVERYTHING_FORMAT, data, parse_options(name)), EVERYTHING_VALUE)
                # strict changes nothing for complete input
                self.assertEqual(pyalpaca.deserialize(EVERYTHING_FORMAT, data, parse_options(name) | int(pyalpaca.options.strict)), EVERYTHING_VALUE)


class Errors(unittest.TestCase):
    def test_bad_format_code(self):
        with self.assertRaisesRegex(ValueError, "unknown format code 'x'"):
            pyalpaca.serialize("ix", [1, 2])
        with self.assertRaisesRegex(ValueError, "unknown format code 'x'"):
            pyalpaca.deserialize("x", b"\x00")

    def test_value_count_mismatch(self):
        with self.assertRaisesRegex(ValueError, "describes 1 fields but 2 values"):
            pyalpaca.serialize("i", [1, 2])
        with self.assertRaisesRegex(ValueError, "more fields than the 1 values"):
            pyalpaca.serialize("ii", [1])

    def test_array_size_mismatch(self):
        with self.assertRaisesRegex(ValueError, "expected 3 array elements, got 2"):
            pyalpaca.serialize("[3B]", [[1, 2]])

    def test_wrong_value_type(self):
        with self.assertRaisesRegex(ValueError, "value 0 is not convertible to int32"):
            pyalpaca.serialize("i", ["not an int"])

    def test_unbalanced_brackets(self):
        with self.assertRaisesRegex(ValueError, "unbalanced"):
            pyalpaca.serialize("[i", [[1]])

    def test_truncated_input(self):
        data = pyalpaca.serialize("is", [1, "hello"])
        # not strict: the missing trailing field is not returned
        self.assertEqual(pyalpaca.deserialize("is", data[:1]), [1])
        # a cut inside the string is always an error
        with self.assertRaises(RuntimeError):
            pyalpaca.deserialize("is", data[:3])
        # strict: ending early is an error
        with self.assertRaisesRegex(RuntimeError, "input ended after 1 bytes, before field 1"):
            pyalpaca.deserialize("is", data[:1], pyalpaca.options.strict)

    def test_bad_vector_size(self):
        with self.assertRaisesRegex(RuntimeError, "invalid vector size"):
            pyalpaca.deserialize("[i]", b"\x7f")

    def test_checksum(self):
        data = pyalpaca.serialize("is", [1, "hello"], pyalpaca.options.with_checksum)
        corrupted = bytes([data[0] ^ 0xFF]) + data[1:]
        with self.assertRaisesRegex(RuntimeError, "checksum mismatch"):
            pyalpaca.deserialize("is", corrupted, pyalpaca.options.with_checksum)
        with self.assertRaisesRegex(RuntimeError, "too short to carry a checksum"):
            pyalpaca.deserialize("is", b"\x01", pyalpaca.options.with_checksum)

    def test_with_version_rejected(self):
        with self.assertRaisesRegex(ValueError, "with_version is not supported"):
            pyalpaca.serialize("i", [1], pyalpaca.options.with_version)
        with self.assertRaisesRegex(ValueError, "with_version is not supported"):
            pyalpaca.deserialize("i", b"\x00", pyalpaca.options.with_version)

    def test_unknown_option_bits(self):
        with self.assertRaisesRegex(ValueError, "unknown option bits"):
            pyalpaca.serialize("i", [1], 1 << 10)


if __name__ == "__main__":
    unittest.main()
