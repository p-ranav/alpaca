#pragma once
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <vector>

namespace alpaca {

namespace detail {

struct blob {
    const uint8_t* start;
    const uint8_t* end;
};

}

// A non-owning view of the struct
class byte_view {
public:
    void push_back(detail::blob b) {
        m_blobs.push_back(std::move(b));
    }

    /// TODO:
    /// Add iterable API

    std::size_t size() const {
      std::size_t result{0};
      std::for_each(m_blobs.begin(), m_blobs.end(), [&result](const detail::blob &b){ result += std::size_t(b.end - b.start); });
      return result;
    }

private:
    std::vector<detail::blob> m_blobs;
};

}