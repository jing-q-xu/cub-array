//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ORDEREDARRAY_H
#define CUB_ARRAY_ORDEREDARRAY_H

#include <cub/array/detail/GenericOrderedArray.h>

namespace detail {
    template<typename ARRAY>
    struct OrderedArrayTrait {
    private:
        struct Holder {
            using ArrayType = ARRAY;
            ARRAY array;
        };

    public:
        using Type = detail::GenericOrderedArray<Holder>;
    };
}

template<typename ARRAY>
struct OrderedArray : detail::OrderedArrayTrait<ARRAY>::Type {};

#endif //CUB_ARRAY_ORDEREDARRAY_H
