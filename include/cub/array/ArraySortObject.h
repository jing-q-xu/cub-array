//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYSORTOBJECT_H
#define CUB_ARRAY_ARRAYSORTOBJECT_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/LessChecker.h>
#include <algorithm>
#include <bitset>
#include "OrderedArrayIndex.h"

template<typename ARRAY>
struct ArraySortObject {
    constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
    using SizeType = DeduceSizeType_t<MAX_SIZE>;
    using BitMap = std::bitset<MAX_SIZE>;
    using ObjectType = typename ARRAY::ObjectType;

    ArraySortObject(ARRAY& array) : array{array} {}

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(LESS&& less) -> SizeType {
        return indices.Sort(array, std::forward<LESS>(less));
    }

    auto Sort() -> SizeType {
        return indices.Sort(array);
    }

    auto DescSort() -> SizeType {
        return indices.DescSort(array);
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(SizeType required, LESS&& less) -> SizeType {
        return indices.Sort(array, required, std::forward<LESS>(less));
    }

    auto Sort(SizeType required) -> SizeType {
        return indices.Sort(array, required);
    }

    auto DescSort(SizeType required) -> SizeType {
        return indices.DescSort(array, required);
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(SizeType required, BitMap enabled, LESS&& less) -> SizeType {
        return indices.Sort(array, required, enabled, std::forward<LESS>(less));
    }

    auto Sort(SizeType required, BitMap enabled) -> SizeType {
        return indices.Sort(array, required, enabled);
    }

    auto DescSort(SizeType required, BitMap enabled) -> SizeType {
        return indices.DescSort(array, required, enabled);
    }

    auto operator[](SizeType n) -> decltype(auto) {
        return (array[indices[n]]);
    }

    auto operator[](SizeType n) const -> decltype(auto) {
        return (array[indices[n]]);
    }

private:
    ARRAY& array;
    OrderedArrayIndex<ARRAY> indices;
};

#endif //CUB_ARRAY_ARRAYSORTOBJECT_H
