#include <alpaca/alpaca.h>

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

int main() {

  /*
    Binary Tree:
          5
         / \
        3   4
       / \
      1   2
  */

  auto const root =
      make_node(5, make_node(3, make_node(1), make_node(2)), make_node(4));

  // serialize
  std::vector<uint8_t> bytes;
  auto bytes_written = alpaca::serialize<Node<int>>(*root, bytes); // 15 bytes

  // bytes:
  // {
  //   0x05 // root = 5
  //   0x01 // 5.has_left = true
  //   0x03 // 5.left = 3
  //   0x01 // 3.has_left = true
  //   0x01 // 3.left = 1
  //   0x00 // 1.left = null
  //   0x00 // 1.right = null
  //   0x01 // 3.has_right = true
  //   0x02 // 3.right = 2
  //   0x00 // 2.has_left = false
  //   0x00 // 2.has_right = false
  //   0x01 // 5.has_right = true
  //   0x04 // 5.right = 4
  //   0x00 // 4.has_left = false
  //   0x00 // 4.has_right = false
  // }

  std::error_code ec;
  auto tree = alpaca::deserialize<Node<int>>(bytes, ec);
  assert((bool)ec == false);
  assert((bool)ec == false);
  assert(tree.data == 5);
  auto &left_subtree_0 = *(tree.left);
  assert(left_subtree_0.data == 3);
  const auto &right_subtree_0 = *(tree.right);
  assert(right_subtree_0.data == 4);
  assert(right_subtree_0.left == nullptr);
  assert(right_subtree_0.right == nullptr);
  const auto &left_subtree_1 = *(left_subtree_0.left);
  assert(left_subtree_1.data == 1);
  assert(left_subtree_1.left == nullptr);
  assert(left_subtree_1.right == nullptr);
  const auto &right_subtree_1 = *(left_subtree_0.right);
  assert(right_subtree_1.data == 2);
  assert(right_subtree_1.left == nullptr);
  assert(right_subtree_1.right == nullptr);
}

#else

int main() {}

#endif