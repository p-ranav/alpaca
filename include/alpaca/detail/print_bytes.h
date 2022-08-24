#pragma once
#include <iomanip>
#include <iostream>
#include <vector>

namespace alpaca {

namespace detail {

static inline void print_bytes(const std::vector<uint8_t> &bytes) {
  std::ios_base::fmtflags f(std::cout.flags());

  // Your code here...
  for (auto &b : bytes) {
    std::cout << std::hex << "0x" << std::setfill('0') << std::setw(2) << (int)b
              << " ";
  }
  std::cout << "\n";

  std::cout.flags(f);
}

} // namespace detail

} // namespace alpaca