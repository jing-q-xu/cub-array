//
// Created by Darwin Yuan on 2021/5/25.
//

#ifndef CUB_ARRAY_OBJECTARRAY_H
#define CUB_ARRAY_OBJECTARRAY_H

#include <cub/base/Placement.h>
#include <cub/array/detail/ArrayHolder.h>
#include <cub/array/ArrayLike.h>

namespace detail {
    template<typename T, std::size_t MAX_NUM>
    struct ObjectArrayTrait
    {
        using Base = ArrayLike<detail::ArrayHolder<T, MAX_NUM, Placement<T>>>;

        struct Type : Base {
            using Base::Base;
        };
    };
}

template<typename T, std::size_t MAX_NUM>
using ObjectArray = typename detail::ObjectArrayTrait<T, MAX_NUM>::Type;


#endif //CUB_ARRAY_OBJECTARRAY_H
