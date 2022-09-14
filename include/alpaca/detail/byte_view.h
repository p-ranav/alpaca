#pragma once
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace alpaca {

namespace detail {

struct blob {
  const void *ptr;
  std::size_t size;
};

} // namespace detail

// A non-owning view of the struct
class byte_view {
public:
  void push_back(detail::blob b) { m_blobs.push_back(std::move(b)); }

  /// TODO:
  /// Add iterable API

  std::size_t size() const {
    std::size_t result{0};
    std::for_each(m_blobs.begin(), m_blobs.end(),
                  [&result](const detail::blob &b) { result += b.size; });
    return result;
  }

private:
  std::vector<detail::blob> m_blobs;
};

} // namespace alpaca