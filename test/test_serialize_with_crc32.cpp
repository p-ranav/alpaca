#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

TEST_CASE("Serialize vector<char>" * test_suite("vector")) {
  struct my_struct {
    int value;
  };

  // sender:

  my_struct s{5};
  auto bytes = serialize(s);

  const auto crc32 = crc32_fast(bytes.data(), bytes.size());

  // append crc32
  detail::append(crc32, bytes);

  REQUIRE(bytes.size() == 5);

  // receiver:

  // compute crc32 of "received" bytes
  auto received = bytes;
  uint32_t crc32_received;
  std::size_t index = received.size() - 4;
  detail::read_bytes<uint32_t, uint32_t>(crc32_received, received,
                                         index); // last 4 bytes

  // check
  REQUIRE(crc32_received == crc32_fast(received.data(), received.size() - 4));
}