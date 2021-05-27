//
// Created by Darwin Yuan on 2021/5/27.
//

#ifndef CUB_ARRAY_DEDUCEBITSETINTTYPE_H
#define CUB_ARRAY_DEDUCEBITSETINTTYPE_H

#include <cstdint>
#include <cstddef>

namespace detail {
    template<std::size_t N>
    auto DeduceBitSetIntType() -> auto {
        static_assert(N <= sizeof(uint64_t) * 8, "too big for int, try std::bitset");
        if constexpr (N <= sizeof(uint8_t) * 8) {
            return uint8_t{};
        } else if constexpr (N <= sizeof(uint16_t) * 8) {
            return uint16_t{};
        } else if constexpr (N <= sizeof(uint32_t) * 8) {
            return uint32_t{};
        } else {
            return uint64_t{};
        }
    }

    template<std::size_t N>
    using DeduceBitSetIntType_t = decltype(DeduceBitSetIntType<N>());

}

#endif //CUB_ARRAY_DEDUCEBITSETINTTYPE_H
