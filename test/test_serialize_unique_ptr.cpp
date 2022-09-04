#include <alpaca/alpaca.h>
#include <doctest.hpp>
using namespace alpaca;

using doctest::test_suite;

#if !defined(_MSC_VER) && !defined(__MINGW32__)

template <class T> struct Node {
  T data;
  std::unique_ptr<Node<T>> left;
  std::unique_ptr<Node<T>> right;
};

template <class T>
auto make_node(T const &value, std::unique_ptr<Node<T>> lhs = nullptr,
               std::unique_ptr<Node<T>> rhs = nullptr) {
  return std::unique_ptr<Node<T>>(
      new Node<T>{value, std::move(lhs), std::move(rhs)});
}

TEST_CASE("Serialize unique_ptr<struct>" * test_suite("unique_ptr")) {

  /*
          5
         / \
        3   4
       / \
      1   2
  */

  auto const root =
      make_node(5, make_node(3, make_node(1), make_node(2)), make_node(4));

  std::vector<uint8_t> bytes;
  serialize(*root, bytes);
  REQUIRE(bytes.size() == 15);
  REQUIRE(bytes[0] == static_cast<uint8_t>(5));  // root = 5
  REQUIRE(bytes[1] == static_cast<uint8_t>(1));  // 5.has_left = true
  REQUIRE(bytes[2] == static_cast<uint8_t>(3));  // 5.left = 3
  REQUIRE(bytes[3] == static_cast<uint8_t>(1));  // 3.has_left = true
  REQUIRE(bytes[4] == static_cast<uint8_t>(1));  // 3.left = 1
  REQUIRE(bytes[5] == static_cast<uint8_t>(0));  // 1.left = null
  REQUIRE(bytes[6] == static_cast<uint8_t>(0));  // 1.right = null
  REQUIRE(bytes[7] == static_cast<uint8_t>(1));  // 3.has_right = true
  REQUIRE(bytes[8] == static_cast<uint8_t>(2));  // 3.right = 2
  REQUIRE(bytes[9] == static_cast<uint8_t>(0));  // 2.has_left = false
  REQUIRE(bytes[10] == static_cast<uint8_t>(0)); // 2.has_right = false
  REQUIRE(bytes[11] == static_cast<uint8_t>(1)); // 5.has_right = true
  REQUIRE(bytes[12] == static_cast<uint8_t>(4)); // 5.right = 4
  REQUIRE(bytes[13] == static_cast<uint8_t>(0)); // 4.has_left = false
  REQUIRE(bytes[14] == static_cast<uint8_t>(0)); // 4.has_right = false
}

#endif