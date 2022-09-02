#include <alpaca/alpaca.h>

int main() {

  struct MyStruct {
    std::map<std::string, std::variant<uint16_t, std::string, bool,
                                       std::vector<std::string>>>
        value;
  };

  MyStruct s{{{"keepalive", true},
              {"port", uint16_t{8080}},
              {"ip_address", std::string{"192.168.8.1"}},
              {"subscriptions",
               std::vector<std::string>{"motor_state", "battery_state"}}}};

  // serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize(s, bytes); // 87 bytes

  // bytes:
  // {
  //   0x04                                                                //
  //   4-element map 0x0a // 10-byte string 0x69 0x70 0x5f 0x61 0x64 0x64 0x72
  //   0x65 0x73 0x73                   // string "ip_address" 0x01 // variant
  //   index = 1, type string 0x0b // 11-byte string 0x31 0x39 0x32 0x2e 0x31
  //   0x36 0x38 0x2e 0x38 0x2e 0x31              // string "192.168.8.1" 0x09
  //   // 9-byte string 0x6b 0x65 0x65 0x70 0x61 0x6c 0x69 0x76 0x65 // string
  //   "keepalive" 0x02 // variant index = 2, type bool 0x01 // bool true 0x04
  //   // 4-byte string 0x70 0x6f 0x72 0x74 // string "port" 0x00 // variant
  //   index = 0, type uint16_t 0x90 0x1f // uint 8080 0x0d // 13-byte string
  //   0x73 0x75 0x62 0x73 0x63 0x72 0x69 0x70 0x74 0x69 0x6f 0x6e 0x73    //
  //   string "subscriptions" 0x03 // variant index = 3, type vector<string>
  //   0x02                                                                //
  //   2-element vector 0x0b // 11-byte string 0x6d 0x6f 0x74 0x6f 0x72 0x5f
  //   0x73 0x74 0x61 0x74 0x65              // string "motor_state" 0x0d //
  //   13-byte string 0x62 0x61 0x74 0x74 0x65 0x72 0x79 0x5f 0x73 0x74 0x61
  //   0x74 0x65    // string "battery_state"
  // }

  std::error_code ec;
  auto recovered = alpaca::deserialize<MyStruct>(bytes, ec);
  assert((bool)ec == false);
  assert((recovered.value == std::map < std::string,
          std::variant<uint16_t, std::string, bool, std::vector<std::string>>{
              {"keepalive", true},
              {"port", uint16_t{8080}},
              {"ip_address", std::string{"192.168.8.1"}},
              {"subscriptions",
               std::vector<std::string>{"motor_state", "battery_state"}}}));
}