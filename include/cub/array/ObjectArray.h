//
// Created by Darwin Yuan on 2021/5/25.
//

#ifndef CUB_ARRAY_OBJECTARRAY_H
#define CUB_ARRAY_OBJECTARRAY_H

#include <cub/base/Placement.h>
#include <cub/array/detail/ArrayHolder.h>
#include <cub/array/detail/ArrayLike.h>

namespace detail {
    template<typename T, std::size_t MAX_NUM, typename = void>
    struct ObjectArrayTrait {
        using Type = detail::ArrayLike<typename detail::ArrayHolderTrait<T>::template Type<MAX_NUM, Placement<T>>>;
    };

    template<typename T, std::size_t MAX_NUM>
    struct ObjectArrayTrait<T, MAX_NUM, std::enable_if_t<std::is_trivially_constructible_v<T>>> {
        using Type = detail::ArrayLike<typename detail::ArrayHolderTrait<T>::template Type<MAX_NUM>>;
    };
}

template<typename T, std::size_t MAX_NUM>
using ObjectArray = typename detail::ObjectArrayTrait<T, MAX_NUM>::Type;

#endif //CUB_ARRAY_OBJECTARRAY_H
