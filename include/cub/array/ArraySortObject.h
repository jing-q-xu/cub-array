//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYSORTOBJECT_H
#define CUB_ARRAY_ARRAYSORTOBJECT_H

#include <cub/base/DeduceSizeType.h>
#include <algorithm>
#include <bitset>

template<typename ARRAY>
struct ArraySortObject {
    constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
    using SizeType = DeduceSizeType_t<MAX_SIZE>;
    using BitMap = std::bitset<MAX_SIZE>;

    ArraySortObject(ARRAY& array) : array{array} {}

    template<typename LESS>
    auto Sort(LESS&& less, SizeType from = 0) -> SizeType {
        InitIndices(from);
        return FullSort(std::forward<LESS>(less));
    }

    template<typename LESS>
    auto Sort(SizeType required, LESS&& less, SizeType from = 0) -> SizeType {
        InitIndices(from);
        return DoSort(required, from, std::forward<LESS>(less));
    }

    template<typename LESS>
    auto Sort(SizeType required, BitMap enabled, LESS&& less, SizeType from = 0) -> SizeType {
        InitIndices(from, enabled);
        return DoSort(required, from, std::forward<LESS>(less));
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
    auto DoSort(SizeType required, SizeType from, LESS&& less) -> SizeType {
        if(required == 0 || from >= array.GetNum()) return 0;
        required = std::min(required, array.GetNum() - from);
        std::partial_sort(indices, indices + required, indices + numOfIndices, [&](auto l, auto r) {
            return less(array[l], array[r]);
        });
        return required;
    }

    auto InitIndices(SizeType from, BitMap enabled) {
        numOfIndices = 0;
        for (auto i=from; i<array.GetNum(); i++) {
            if(enabled.test(i)) {
                indices[numOfIndices++] = i;
            }
        }
    }

    auto InitIndices(SizeType from) {
        numOfIndices = 0;
        for (auto i=from; i<array.GetNum(); i++) {
            indices[numOfIndices++] = i;
        }
    }

private:
    ARRAY& array;
    SizeType indices[MAX_SIZE];
    SizeType numOfIndices;
};

#endif //CUB_ARRAY_ARRAYSORTOBJECT_H
