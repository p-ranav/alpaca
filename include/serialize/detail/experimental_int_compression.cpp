#include <cstdint>
#include <vector>
#include <iostream>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) var = var | 1 << pos
#define RESET_BIT(var,pos) var = var & ~(1 << pos)

#include <iostream>
#include <iomanip>

void print_bytes(const std::vector<uint8_t>& bytes) {
    std::ios_base::fmtflags f( std::cout.flags() );

    //Your code here...
    for (auto& b: bytes) {
        std::cout << std::hex
            << "0x" 
            << std::setfill('0')
            << std::setw(2)
            << (int)b
            << " ";
    }
    std::cout << "\n";

    std::cout.flags( f );
}

template <typename int_t = uint64_t>
bool encode_varint_firstbyte_6(int_t& value, std::vector<uint8_t> &output) {
  int_t copy = value;
  uint8_t octet = 0;
  if (value < 0) {
    copy = std::abs(copy);
    SET_BIT(octet, 7);
  }
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  if (copy > 63) {
    //|128: Set the next byte flag
    octet |= ((uint8_t)(copy & 63)) | 64;
    output.push_back(octet);
    return true;
  }
  else {
    octet |= ((uint8_t)(copy & 63));
    output.push_back(octet);
    return false;
  }
}

template <typename int_t = uint64_t>
void encode_varint_6(int_t value, std::vector<uint8_t> &output) {
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 63) {
    //|128: Set the next byte flag
    output.push_back(((uint8_t)(value & 63)) | 64);
    // Remove the seven bits we just wrote
    value >>= 6;
  }
  output.push_back(((uint8_t)value) & 63);
}

template <typename int_t = uint64_t>
std::pair<int_t, bool> decode_varint_firstbyte_6(std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  uint8_t octet = 0;
  bool negative = false;
  if (CHECK_BIT(input[current_index], 7)) {
    // negative number
    RESET_BIT(input[current_index], 7);
    negative = true;
  }
  RESET_BIT(input[current_index], 6);
  octet |= input[current_index++] & 63;
  return {static_cast<int_t>(octet), negative};
}

template <typename int_t = uint64_t>
int_t decode_varint_6(const std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  int_t ret = 0;
  for (std::size_t i = 0; i < sizeof(int_t); ++i) {
    ret |= (static_cast<int_t>(input[current_index + i] & 63)) << (6 * i);
    // If the next-byte flag is set
    if (!(input[current_index + i] & 64)) {
      current_index += i + 1;
      break;
    }
  }
  return ret;
}

template <typename int_t = uint64_t>
void encode_varint_7(int_t value, std::vector<uint8_t> &output) {
  if (value < 0) {
    value = std::abs(value);
  }
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 127) {
    //|128: Set the next byte flag
    output.push_back(((uint8_t)(value & 127)) | 128);
    // Remove the seven bits we just wrote
    value >>= 7;
  }
  output.push_back(((uint8_t)value) & 127);
}

template <typename int_t = uint64_t>
int_t decode_varint_7(const std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  int_t ret = 0;
  for (std::size_t i = 0; i < sizeof(int_t); ++i) {
    ret |= (static_cast<int_t>(input[current_index + i] & 127)) << (7 * i);
    // If the next-byte flag is set
    if (!(input[current_index + i] & 128)) {
      current_index += i + 1;
      break;
    }
  }
  return ret;
}

int main() {
    {
        std::vector<uint8_t> bytes;
        auto value = 12, recovered = 0;
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        recovered = decode_varint_7<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << recovered << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = 123, recovered = 0;
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        recovered = decode_varint_7<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << recovered << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = 1234, recovered = 0;
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        recovered = decode_varint_7<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << recovered << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = 12345, recovered = 0;
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        recovered = decode_varint_7<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << recovered << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = 12345, recovered = 0;
        encode_varint_6(value, bytes);
        std::size_t current_index = 0;
        recovered = decode_varint_6<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << recovered << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = 12345, recovered = 0;
        // first octet
        encode_varint_firstbyte_6(value, bytes);
        // rest of the octets
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<decltype(value)>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // decode rest of the bytes
        partial |= decode_varint_7<decltype(value)>(bytes, current_index);
        std::cout << bytes.size() << " " << partial << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        auto value = -12345, recovered = 0;
        // first octet
        encode_varint_firstbyte_6(value, bytes);
        // rest of the octets
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<decltype(value)>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // decode rest of the bytes
        partial |= decode_varint_7<decltype(value)>(bytes, current_index);
        if (is_negative) {
            partial *= -1;
        }
        std::cout << bytes.size() << " " << (int)partial << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        int64_t value = 12345678910111314, recovered = 0;
        // first octet
        encode_varint_firstbyte_6(value, bytes);
        // rest of the octets
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<int64_t>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // decode rest of the bytes
        partial |= decode_varint_7<int64_t>(bytes, current_index);
        if (is_negative) {
            partial *= -1;
        }
        std::cout << bytes.size() << " " << partial << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        int64_t value = -12345678910111314, recovered = 0;
        // first octet
        encode_varint_firstbyte_6(value, bytes);
        // rest of the octets
        encode_varint_7(value, bytes);
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<decltype(value)>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // decode rest of the bytes
        partial |= decode_varint_7<decltype(value)>(bytes, current_index);
        if (is_negative) {
            partial *= -1;
        }
        std::cout << bytes.size() << " " << partial << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        int8_t value = -5, recovered = 0;
        // first octet
        if (encode_varint_firstbyte_6(value, bytes)) {
          // rest of the octets
          encode_varint_7(value, bytes);
        }
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<decltype(value)>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // // decode rest of the bytes
        // partial |= decode_varint_7<decltype(value)>(bytes, current_index);
        if (is_negative) {
          partial *= -1;
        }
        std::cout << bytes.size() << " " << (int)partial << "\n";
    }

    {
        std::vector<uint8_t> bytes;
        int16_t value = -12345, recovered = 0;
        // first octet
        if (encode_varint_firstbyte_6(value, bytes)) {
          // rest of the octets
          encode_varint_7(value, bytes);
        }
        std::size_t current_index = 0;
        // decode first byte
        auto pair = decode_varint_firstbyte_6<decltype(value)>(bytes, current_index);
        auto partial = pair.first;
        auto is_negative = pair.second;
        // // decode rest of the bytes
        // partial |= decode_varint_7<decltype(value)>(bytes, current_index);
        if (is_negative) {
          partial *= -1;
        }
        std::cout << bytes.size() << " " << (int)partial << "\n";
    }
}

/*
1 12
1 123
2 1234
2 12345
3 12345
3 12345
3 -12345
9 12345678910111314
9 -12345678910111314
*/
