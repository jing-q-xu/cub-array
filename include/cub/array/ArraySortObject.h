//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYSORTOBJECT_H
#define CUB_ARRAY_ARRAYSORTOBJECT_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/LessChecker.h>
#include <algorithm>
#include <bitset>

template<typename ARRAY>
struct ArraySortObject {
    constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
    using SizeType = DeduceSizeType_t<MAX_SIZE>;
    using BitMap = std::bitset<MAX_SIZE>;
    using ObjectType = typename ARRAY::ObjectType;

    ArraySortObject(ARRAY& array) : array{array} {}

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(LESS&& less) -> SizeType {
        InitIndices();
        return FullSort(std::forward<LESS>(less));
    }

    auto Sort() -> SizeType {
        return Sort([](auto&& l, auto&& r) { return l < r; });
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(SizeType required, LESS&& less) -> SizeType {
        InitIndices();
        return DoSort(required, std::forward<LESS>(less));
    }

    auto Sort(SizeType required) -> SizeType {
        return Sort([](auto&& l, auto&& r) { return l < r; }, required);
    }

    template<typename LESS, __lEsS_cHeCkEr>
    auto Sort(SizeType required, BitMap enabled, LESS&& less) -> SizeType {
        InitIndices(enabled);
        return DoSort(required, std::forward<LESS>(less));
    }

    auto Sort(SizeType required, BitMap enabled) -> SizeType {
        return Sort([](auto&& l, auto&& r) { return l < r; }, required, enabled);
    }

    auto operator[](SizeType n) -> decltype(auto) {
        return (array[indices[n]]);
    }

    auto operator[](SizeType n) const -> decltype(auto) {
        return (array[indices[n]]);
    }

private:
    template<typename LESS>
    auto FullSort(LESS&& less) -> SizeType {
        std::stable_sort(indices, indices + numOfIndices, [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return numOfIndices;
    }

    template<typename LESS>
    auto DoSort(SizeType required, LESS&& less) -> SizeType {
        required = std::min(required, array.GetNum());
        if(required == 0) return 0;
        std::partial_sort(indices, indices + required, indices + numOfIndices, [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return required;
    }

    auto InitIndices(BitMap enabled) {
        numOfIndices = 0;
        for (auto i=0; i<array.GetNum(); i++) {
            if(enabled.test(i)) {
                indices[numOfIndices++] = i;
            }
        }
    }

    auto InitIndices() {
        numOfIndices = 0;
        for (auto i=0; i<array.GetNum(); i++) {
            indices[numOfIndices++] = i;
        }
    }

private:
    ARRAY& array;
    SizeType indices[MAX_SIZE];
    SizeType numOfIndices;
};

#endif //CUB_ARRAY_ARRAYSORTOBJECT_H
