#pragma once
#include <string>
#include <vector>
#include "minecraft_save.h"

namespace alpaca {

namespace benchmark {

struct Address {
    uint32_t x0;
    uint32_t x1;
    uint32_t x2;
    uint32_t x3;
};

struct Log {
    Address address;
    std::string identity;
    std::string userid;
    std::string date;
    std::string request;
    uint32_t code;
    uint64_t size;
};

struct Logs {
    std::vector<Log> logs;
};

template <typename Engine>
auto generate_address(Engine &eng) {
    std::uniform_int_distribution<uint32_t> uint_distr(0, 20000);
    return Address {
        uint_distr(eng),
        uint_distr(eng),
        uint_distr(eng),
        uint_distr(eng)
    };
}

template <typename Engine>
auto generate_log(Engine &eng) {
    std::uniform_int_distribution<uint32_t> uint32_distr(0, 20000);
    std::uniform_int_distribution<uint64_t> uint64_distr(0, 20000);

    return Log {
        generate_address(eng),
        generate_string(12),
        generate_string(8),
        generate_string(16),
        generate_string(32),
        uint32_distr(eng),
        uint64_distr(eng)
    };
}

template <typename Engine>
auto generate_logs(Engine &eng) {
    constexpr auto size = 10000;
    Logs result{};

    for (std::size_t i = 0; i < size; ++i) {
        result.logs.push_back(generate_log(eng));
    }

    return result;
}

}

}