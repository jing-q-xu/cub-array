//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_GENERICORDEREDARRAY_H
#define CUB_ARRAY_GENERICORDEREDARRAY_H

#include <cub/array/OrderedArrayIndex.h>

namespace detail {
    template<typename ARRAY_HOLDER>
    struct GenericOrderedArray : private ARRAY_HOLDER {
        using ARRAY = typename ARRAY_HOLDER::ArrayType;
        constexpr static auto MAX_SIZE = ARRAY::MAX_SIZE;
        using SizeType = DeduceSizeType_t<MAX_SIZE>;
        using BitMap = BitSet<MAX_SIZE>;
        using ObjectType = typename ARRAY::ObjectType;
        using Parent = ARRAY_HOLDER;

        using Parent::Parent;

        template<typename LESS, __lEsS_cHeCkEr>
        auto Sort(LESS&& less) -> SizeType {
            return indices.Sort(Parent::array, std::forward<LESS>(less));
        }

        auto Sort() -> SizeType {
            return indices.Sort(Parent::array);
        }

        auto DescSort() -> SizeType {
            return indices.DescSort(Parent::array);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto Sort(SizeType required, LESS&& less) -> SizeType {
            return indices.Sort(Parent::array, required, std::forward<LESS>(less));
        }

        auto Sort(SizeType required) -> SizeType {
            return indices.Sort(Parent::array, required);
        }

        auto DescSort(SizeType required) -> SizeType {
            return indices.DescSort(Parent::array, required);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto Sort(SizeType required, BitMap enabled, LESS&& less) -> SizeType {
            return indices.Sort(Parent::array, required, enabled, std::forward<LESS>(less));
        }

        auto Sort(SizeType required, BitMap enabled) -> SizeType {
            return indices.Sort(Parent::array, required, enabled);
        }

        auto DescSort(SizeType required, BitMap enabled) -> SizeType {
            return indices.DescSort(Parent::array, required, enabled);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto Sort(BitMap enabled, LESS&& less) -> SizeType {
            return indices.Sort(Parent::array, enabled, std::forward<LESS>(less));
        }

        auto Sort(BitMap enabled) -> SizeType {
            return indices.Sort(Parent::array, enabled);
        }

        auto DescSort(BitMap enabled) -> SizeType {
            return indices.DescSort(Parent::array, enabled);
        }

        auto operator[](SizeType n) -> decltype(auto) {
            return (Parent::array[indices[n]]);
        }

        auto operator[](SizeType n) const -> decltype(auto) {
            return (Parent::array[indices[n]]);
        }

        auto operator*() const -> decltype(auto) {
            return (Parent::array);
        }

        auto operator*() -> decltype(auto) {
            return (Parent::array);
        }

        auto operator->() const -> auto {
            return &(Parent::array);
        }

        auto operator->() -> auto {
            return &(Parent::array);
        }

    protected:
        auto GetArray() -> decltype(auto) {
            return (Parent::array);
        }
    private:
        OrderedArrayIndex<ARRAY> indices;
    };
}

#endif //CUB_ARRAY_GENERICORDEREDARRAY_H
