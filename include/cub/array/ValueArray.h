//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_VALUEARRAY_H
#define CUB_ARRAY_VALUEARRAY_H

#include <cub/base/Placement.h>
#include <cub/array/detail/ArrayHolder.h>
#include <cub/array/detail/ArrayLike.h>

template<typename T, std::size_t MAX_NUM>
using ValueArray = detail::ArrayLike<detail::ArrayHolder<T, MAX_NUM>>;

#endif //CUB_ARRAY_VALUEARRAY_H
