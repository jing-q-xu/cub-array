//
// Created by Darwin Yuan on 2021/5/27.
//

#ifndef CUB_ARRAY_BITSET_H
#define CUB_ARRAY_BITSET_H

#include <cub/base/IntBitSet.h>
#include <bitset>

namespace detail {
    template<std::size_t N>
    auto DeduceBitSetType() -> auto {
        if constexpr (N <= sizeof(std::size_t) * 8) {
            return IntBitSet<N>{};
        } else {
            return std::bitset<N>{};
        }
    }
}

template<std::size_t N>
using BitSet = decltype(detail::DeduceBitSetType<N>());

#endif //CUB_ARRAY_BITSET_H
