//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_SIZEOFARRAY_H
#define CUB_ARRAY_SIZEOFARRAY_H

#include <cstddef>

template<typename T, std::size_t SIZE>
constexpr auto SizeOfArray(T (&)[SIZE]) -> auto {
    return SIZE;
}

template<typename T>
constexpr auto SizeOfArray(std::nullptr_t) -> auto {
    return 0;
}

#endif //CUB_ARRAY_SIZEOFARRAY_H
