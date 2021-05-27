//
// Created by Darwin Yuan on 2021/5/27.
//

#ifndef CUB_ARRAY_UNIONTRAIT_H
#define CUB_ARRAY_UNIONTRAIT_H

#include <type_traits>

namespace detail {
    template<typename T, typename = void>
    struct UnionTrait {
        union Type {
            Type() {}
            T obj;
            char storage[sizeof(T)];
        };
    };

    template<typename T>
    struct UnionTrait<T, std::enable_if_t<!std::is_trivially_destructible_v<T>>> {
        union Type {
            Type() {}
            ~Type() {}
            T obj;
            char storage[sizeof(T)];
        };
    };
}

#endif //CUB_ARRAY_UNIONTRAIT_H
