#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Deserialize variant" * test_suite("variant")) {
  struct my_struct {
    std::variant<int, std::string> value;
  };

  {
    std::array<uint8_t, 20> bytes;
    {
      my_struct s{5};
      serialize(s, bytes);
    }
    {
      std::error_code ec;
      auto result = deserialize<my_struct>(bytes, ec);
      REQUIRE((bool)ec == false);
      REQUIRE(std::get<0>(result.value) == 5);
    }
  }

  {
    std::array<uint8_t, 20> bytes;
    {
      my_struct s{"Hello"};
      serialize(s, bytes);
    }
    {
      std::error_code ec;
      auto result = deserialize<my_struct>(bytes, ec);
      REQUIRE((bool)ec == false);
      REQUIRE(std::get<1>(result.value) == "Hello");
    }
  }
}

TEST_CASE("Deserialize map<string, variant>" * test_suite("variant")) {
  struct my_struct {
    std::map<std::string, std::variant<uint16_t, std::string, bool,
                                       std::vector<std::string>>>
        config;
  };

  std::array<uint8_t, 100> bytes;
  {
    my_struct s{{{"keepalive", true},
                 {"port", uint16_t{8080}},
                 {"ip_address", std::string{"192.168.8.1"}},
                 {"subscriptions",
                  std::vector<std::string>{"motor_state", "battery_state"}}}};
    serialize(s, bytes);
  }

  {
    std::error_code ec;
    auto result = deserialize<my_struct>(bytes, ec);
    REQUIRE((bool)ec == false);
    REQUIRE(result.config.size() == 4);
    auto &value = result.config.at("keepalive");
    REQUIRE(std::get<2>(value) == true);
    value = result.config.at("port");
    REQUIRE(std::get<0>(value) == 8080);
    value = result.config.at("ip_address");
    REQUIRE(std::get<1>(value) == std::string{"192.168.8.1"});
    value = result.config.at("subscriptions");
    REQUIRE(std::get<3>(value) ==
            std::vector<std::string>{"motor_state", "battery_state"});
  }
}