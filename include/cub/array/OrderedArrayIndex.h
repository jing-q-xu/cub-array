//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ORDEREDARRAYINDEX_H
#define CUB_ARRAY_ORDEREDARRAYINDEX_H

#include <cub/base/DeduceSizeType.h>
#include <cub/base/BitSet.h>
#include <cub/array/detail/LessChecker.h>
#include <cub/array/ValueArray.h>
#include <cstdint>
#include <algorithm>

template<typename ARRAY>
struct OrderedArrayIndex {
    constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
    using SizeType = DeduceSizeType_t<MAX_SIZE>;
    using BitMap = BitSet<MAX_SIZE>;
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
        return Sort(array, required, DEFAULT_LESS_THAN);
    }

    auto DescSort(ARRAY const& array, SizeType required) -> SizeType {
        return Sort(array, required, DEFAULT_GREATER_THAN);
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

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(ARRAY const& array, BitMap enabled, LESS&& less) -> SizeType {
        return Sort(array, array.GetNum(), enabled, std::forward<LESS>(less));
    }

    auto Sort(ARRAY const& array, BitMap enabled) -> SizeType {
        return Sort(array, enabled, DEFAULT_LESS_THAN);
    }

    auto DescSort(ARRAY const& array, BitMap enabled) -> SizeType {
        return Sort(array, enabled, DEFAULT_GREATER_THAN);
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
        SizeType* begin = &indices[0];
        std::stable_sort(begin, begin + indices.GetNum(), [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return indices.GetNum();
    }

    template<typename LESS>
    auto DoSort(ARRAY const& array, SizeType required, LESS&& less) -> SizeType {
        required = std::min(required, indices.GetNum());
        if(required == 0) return 0;
        SizeType* begin = &indices[0];
        std::partial_sort(begin, begin + required, begin + indices.GetNum(), [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return required;
    }

    auto InitIndices(ARRAY const& array, BitMap enabled) {
        indices.Clear();
        if(enabled.none()) return;
        BitMap remain = enabled;
        for (auto i=0; i<array.GetNum(); i++) {
            if(remain.test(i)) {
                indices.Append(i);
                remain.reset(i);
                if(remain.none()) break;
            }
        }
    }

    auto InitIndices(ARRAY const& array) {
        indices.Clear();
        for (auto i=0; i<array.GetNum(); i++) {
            indices.Append(i);
        }
    }

private:
    ValueArray<SizeType, MAX_SIZE> indices;
};

#endif //CUB_ARRAY_ORDEREDARRAYINDEX_H
