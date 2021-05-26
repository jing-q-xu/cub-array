//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_LESSCHECKER_H
#define CUB_ARRAY_LESSCHECKER_H

#include <type_traits>
#define __lEsS_cHeCkEr std::enable_if_t<std::is_invocable_r_v<bool, LESS, ObjectType const&, ObjectType const&>, bool> = true

constexpr auto DEFAULT_LESS_THAN = [](auto&& l, auto && r) { return l < r; };
constexpr auto DEFAULT_GREATER_THAN = [](auto&& l, auto && r) { return r < l; };

#endif //CUB_ARRAY_LESSCHECKER_H
