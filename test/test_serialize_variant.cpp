#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize variant" * test_suite("variant")) {
  struct my_struct {
    std::variant<int, std::string> value;
  };

  {
    my_struct s{5};
    std::vector<uint8_t> bytes;
    serialize(s, bytes);
    REQUIRE(bytes.size() == 2);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0)); // index = 0, int
    REQUIRE(bytes[1] == static_cast<uint8_t>(5));
  }

  {
    my_struct s{"Hello!"};
    std::vector<uint8_t> bytes;
    serialize(s, bytes);
    REQUIRE(bytes.size() == 8);
    REQUIRE(bytes[0] == static_cast<uint8_t>(1)); // index = 1, string
    REQUIRE(bytes[1] == static_cast<uint8_t>(6)); // string size = 6
    REQUIRE(bytes[2] == static_cast<uint8_t>('H'));
    REQUIRE(bytes[3] == static_cast<uint8_t>('e'));
    REQUIRE(bytes[4] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[5] == static_cast<uint8_t>('l'));
    REQUIRE(bytes[6] == static_cast<uint8_t>('o'));
    REQUIRE(bytes[7] == static_cast<uint8_t>('!'));
  }
}

TEST_CASE("Serialize map<string, variant>" * test_suite("variant")) {
  struct my_struct {
    std::map<std::string, std::variant<uint16_t, std::string, bool,
                                       std::vector<std::string>>>
        config;
  };

  {
    my_struct s{{{"keepalive", true},
                 {"port", uint16_t{8080}},
                 {"ip_address", std::string{"192.168.8.1"}},
                 {"subscriptions",
                  std::vector<std::string>{"motor_state", "battery_state"}}}};
    std::vector<uint8_t> bytes;
    serialize(s, bytes);

    REQUIRE(bytes.size() == 87);
    REQUIRE(bytes[0] == static_cast<uint8_t>(0x04)); // 4-element map
    REQUIRE(bytes[1] == static_cast<uint8_t>(0x0a)); // 10-byte string
    REQUIRE(bytes[2] == static_cast<uint8_t>(0x69)); // "ip_address"
    REQUIRE(bytes[3] == static_cast<uint8_t>(0x70));
    REQUIRE(bytes[4] == static_cast<uint8_t>(0x5f));
    REQUIRE(bytes[5] == static_cast<uint8_t>(0x61));
    REQUIRE(bytes[6] == static_cast<uint8_t>(0x64));
    REQUIRE(bytes[7] == static_cast<uint8_t>(0x64));
    REQUIRE(bytes[8] == static_cast<uint8_t>(0x72));
    REQUIRE(bytes[9] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[10] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[11] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[12] ==
            static_cast<uint8_t>(0x01)); // variant_index = 1 (string)
    REQUIRE(bytes[13] == static_cast<uint8_t>(0x0b)); // 11-byte string
    REQUIRE(bytes[14] == static_cast<uint8_t>(0x31)); // "192.168.8.1"
    REQUIRE(bytes[15] == static_cast<uint8_t>(0x39));
    REQUIRE(bytes[16] == static_cast<uint8_t>(0x32));
    REQUIRE(bytes[17] == static_cast<uint8_t>(0x2e));
    REQUIRE(bytes[18] == static_cast<uint8_t>(0x31));
    REQUIRE(bytes[19] == static_cast<uint8_t>(0x36));
    REQUIRE(bytes[20] == static_cast<uint8_t>(0x38));
    REQUIRE(bytes[21] == static_cast<uint8_t>(0x2e));
    REQUIRE(bytes[22] == static_cast<uint8_t>(0x38));
    REQUIRE(bytes[23] == static_cast<uint8_t>(0x2e));
    REQUIRE(bytes[24] == static_cast<uint8_t>(0x31));

    REQUIRE(bytes[25] == static_cast<uint8_t>(0x09)); // 9-byte string
    REQUIRE(bytes[26] == static_cast<uint8_t>(0x6b)); // "keepalive"
    REQUIRE(bytes[27] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[28] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[29] == static_cast<uint8_t>(0x70));
    REQUIRE(bytes[30] == static_cast<uint8_t>(0x61));
    REQUIRE(bytes[31] == static_cast<uint8_t>(0x6c));
    REQUIRE(bytes[32] == static_cast<uint8_t>(0x69));
    REQUIRE(bytes[33] == static_cast<uint8_t>(0x76));
    REQUIRE(bytes[34] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[35] ==
            static_cast<uint8_t>(0x02)); // variant_index = 2 (bool)
    REQUIRE(bytes[36] == static_cast<uint8_t>(0x01)); // bool true

    REQUIRE(bytes[37] == static_cast<uint8_t>(0x04)); // 4-byte string
    REQUIRE(bytes[38] == static_cast<uint8_t>(0x70)); // "port"
    REQUIRE(bytes[39] == static_cast<uint8_t>(0x6f));
    REQUIRE(bytes[40] == static_cast<uint8_t>(0x72));
    REQUIRE(bytes[41] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[42] ==
            static_cast<uint8_t>(0x00)); // variant_index = 0 (uint16_t)

    // serializes in little endian since no option override is provided
    REQUIRE(bytes[43] == static_cast<uint8_t>(0x90)); // port: 8080
    REQUIRE(bytes[44] == static_cast<uint8_t>(0x1f));

    REQUIRE(bytes[45] == static_cast<uint8_t>(0x0d)); // 13-byte string
    REQUIRE(bytes[46] == static_cast<uint8_t>(0x73)); // "subscriptions"
    REQUIRE(bytes[47] == static_cast<uint8_t>(0x75));
    REQUIRE(bytes[48] == static_cast<uint8_t>(0x62));
    REQUIRE(bytes[49] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[50] == static_cast<uint8_t>(0x63));
    REQUIRE(bytes[51] == static_cast<uint8_t>(0x72));
    REQUIRE(bytes[52] == static_cast<uint8_t>(0x69));
    REQUIRE(bytes[53] == static_cast<uint8_t>(0x70));
    REQUIRE(bytes[54] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[55] == static_cast<uint8_t>(0x69));
    REQUIRE(bytes[56] == static_cast<uint8_t>(0x6f));
    REQUIRE(bytes[57] == static_cast<uint8_t>(0x6e));
    REQUIRE(bytes[58] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[59] ==
            static_cast<uint8_t>(0x03)); // variant_index = 3 (vector)
    REQUIRE(bytes[60] == static_cast<uint8_t>(0x02)); // 2-element vector
    REQUIRE(bytes[61] == static_cast<uint8_t>(0x0b)); // 11-byte string
    REQUIRE(bytes[62] == static_cast<uint8_t>(0x6d)); // "motor_state"
    REQUIRE(bytes[63] == static_cast<uint8_t>(0x6f));
    REQUIRE(bytes[64] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[65] == static_cast<uint8_t>(0x6f));
    REQUIRE(bytes[66] == static_cast<uint8_t>(0x72));
    REQUIRE(bytes[67] == static_cast<uint8_t>(0x5f));
    REQUIRE(bytes[68] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[69] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[70] == static_cast<uint8_t>(0x61));
    REQUIRE(bytes[71] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[72] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[73] == static_cast<uint8_t>(0x0d)); // 13-byte string
    REQUIRE(bytes[74] == static_cast<uint8_t>(0x62)); // "battery_state"
    REQUIRE(bytes[75] == static_cast<uint8_t>(0x61));
    REQUIRE(bytes[76] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[77] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[78] == static_cast<uint8_t>(0x65));
    REQUIRE(bytes[79] == static_cast<uint8_t>(0x72));
    REQUIRE(bytes[80] == static_cast<uint8_t>(0x79));
    REQUIRE(bytes[81] == static_cast<uint8_t>(0x5f));
    REQUIRE(bytes[82] == static_cast<uint8_t>(0x73));
    REQUIRE(bytes[83] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[84] == static_cast<uint8_t>(0x61));
    REQUIRE(bytes[85] == static_cast<uint8_t>(0x74));
    REQUIRE(bytes[86] == static_cast<uint8_t>(0x65));
  }
}