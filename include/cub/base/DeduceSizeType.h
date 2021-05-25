//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_DEDUCESIZETYPE_H
#define CUB_ARRAY_DEDUCESIZETYPE_H

#include <limits>
#include <cstdint>

namespace detail {
    template<std::size_t MAX_NUM>
    auto DeduceSizeType() -> auto {
        if constexpr(MAX_NUM <= std::numeric_limits<uint8_t>::max()) {
            return uint8_t{};
        } else if constexpr (MAX_NUM <= std::numeric_limits<uint16_t>::max()) {
            return uint16_t{};
        } else if constexpr (MAX_NUM <= std::numeric_limits<uint32_t>::max()) {
            return uint32_t{};
        } else {
            return std::size_t{};
        }
    }
}

template<std::size_t MAX_NUM>
using DeduceSizeType_t = decltype(detail::DeduceSizeType<MAX_NUM>());


#endif //CUB_ARRAY_DEDUCESIZETYPE_H
