//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYSORTOBJECT_H
#define CUB_ARRAY_ARRAYSORTOBJECT_H

#include <cub/array/detail/GenericOrderedArray.h>

namespace detail {
    template<typename ARRAY>
    struct ArraySortObjectTrait {
    private:
        struct Holder {
            using ArrayType = ARRAY;

            Holder(ARRAY& array) : array{array} {}

            ARRAY& array;
        };

    public:
        using Type = detail::GenericOrderedArray<Holder>;
    };
}

template<typename ARRAY>
struct ArraySortObject : detail::ArraySortObjectTrait<ARRAY>::Type {
    using Parent = typename detail::ArraySortObjectTrait<ARRAY>::Type;
    using Parent::Parent;
};

template<typename ARRAY>
ArraySortObject(ARRAY&) -> ArraySortObject<ARRAY>;

#endif //CUB_ARRAY_ARRAYSORTOBJECT_H
