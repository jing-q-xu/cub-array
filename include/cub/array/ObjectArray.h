//
// Created by Darwin Yuan on 2021/5/25.
//

#ifndef CUB_ARRAY_OBJECTARRAY_H
#define CUB_ARRAY_OBJECTARRAY_H

#include <cub/base/Placement.h>
#include <cub/array/detail/ArrayHolder.h>
#include <cub/array/detail/ArrayLike.h>

template<typename T, std::size_t MAX_NUM>
using ObjectArray = detail::ArrayLike<typename detail::ArrayHolderTrait<T>::template Type<MAX_NUM, Placement<T>>>;

#endif //CUB_ARRAY_OBJECTARRAY_H
