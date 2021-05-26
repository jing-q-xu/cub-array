//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ORDEREDOBJECTARRAY_H
#define CUB_ARRAY_ORDEREDOBJECTARRAY_H

#include <cub/array/OrderedArray.h>
#include <cub/array/ObjectArray.h>

template<typename OBJ, std::size_t MAX_SIZE>
struct OrderedObjectArray : OrderedArray<ObjectArray<OBJ, MAX_SIZE>> {};

#endif //CUB_ARRAY_ORDEREDOBJECTARRAY_H
