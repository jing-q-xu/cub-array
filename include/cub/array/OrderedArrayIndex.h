//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ORDEREDARRAYINDEX_H
#define CUB_ARRAY_ORDEREDARRAYINDEX_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/LessChecker.h>
#include <cstdint>
#include <algorithm>
#include <bitset>

template<typename ARRAY>
struct OrderedArrayIndex {
    constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
    using SizeType = DeduceSizeType_t<MAX_SIZE>;
    using BitMap = std::bitset<MAX_SIZE>;
    using ObjectType = typename ARRAY::ObjectType;

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(ARRAY const& array, LESS&& less) -> SizeType {
        InitIndices(array);
        return FullSort(array, std::forward<LESS>(less));
    }

    auto Sort(ARRAY const& array) -> SizeType {
        return Sort(array, DEFAULT_LESS_THAN);
    }

    auto DescSort(ARRAY const& array) -> SizeType {
        return Sort(array, DEFAULT_GREATER_THAN);
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(ARRAY const& array, SizeType required, LESS&& less) -> SizeType {
        InitIndices(array);
        return DoSort(array, required, std::forward<LESS>(less));
    }

    auto Sort(ARRAY const& array, SizeType required) -> SizeType {
        return Sort(array, DEFAULT_LESS_THAN, required);
    }

    auto DescSort(ARRAY const& array, SizeType required) -> SizeType {
        return Sort(array, DEFAULT_GREATER_THAN, required);
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(ARRAY const& array, SizeType required, BitMap enabled, LESS&& less) -> SizeType {
        InitIndices(array, enabled);
        return DoSort(array, required, std::forward<LESS>(less));
    }

    auto Sort(ARRAY const& array, SizeType required, BitMap enabled) -> SizeType {
        return Sort(array, DEFAULT_LESS_THAN, required, enabled);
    }

    auto DescSort(ARRAY const& array, SizeType required, BitMap enabled) -> SizeType {
        return Sort(DEFAULT_GREATER_THAN, required, enabled);
    }

    auto operator[](SizeType n) -> SizeType {
        return indices[n];
    }

    auto operator[](SizeType n) const -> SizeType {
        return indices[n];
    }

private:
    template<typename LESS>
    auto FullSort(ARRAY const& array, LESS&& less) -> SizeType {
        std::stable_sort(indices, indices + numOfIndices, [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return numOfIndices;
    }

    template<typename LESS>
    auto DoSort(ARRAY const& array, SizeType required, LESS&& less) -> SizeType {
        required = std::min(required, array.GetNum());
        if(required == 0) return 0;
        std::partial_sort(indices, indices + required, indices + numOfIndices, [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return required;
    }

    auto InitIndices(ARRAY const& array, BitMap enabled) {
        numOfIndices = 0;
        for (auto i=0; i<array.GetNum(); i++) {
            if(enabled.test(i)) {
                indices[numOfIndices++] = i;
            }
        }
    }

    auto InitIndices(ARRAY const& array) {
        numOfIndices = 0;
        for (auto i=0; i<array.GetNum(); i++) {
            indices[numOfIndices++] = i;
        }
    }

private:
    SizeType indices[MAX_SIZE];
    SizeType numOfIndices;
};

#endif //CUB_ARRAY_ORDEREDARRAYINDEX_H
