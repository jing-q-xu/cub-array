//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYHOLDER_H
#define CUB_ARRAY_ARRAYHOLDER_H

#include <cub/base/DeduceSizeType.h>
#include <cstdint>

namespace detail {
    template<typename T, std::size_t MAX_NUM, typename OBJ = T>
    struct ArrayHolder {
        static constexpr auto MAX_SIZE = MAX_NUM;
        using SizeType = DeduceSizeType_t<MAX_NUM>;

        OBJ objs[MAX_SIZE];
        SizeType num{};
    };
}

#endif //CUB_ARRAY_ARRAYHOLDER_H
