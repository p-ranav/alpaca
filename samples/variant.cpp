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

  // deserialize
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
