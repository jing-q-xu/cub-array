//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_PREDCHECKER_H
#define CUB_ARRAY_PREDCHECKER_H

#include <type_traits>

#define __lEsS_cHeCkEr std::enable_if_t<std::is_invocable_r_v<bool, LESS, ObjectType const&, ObjectType const&>, bool> = true
#define __pReD_cHeCkEr std::enable_if_t<std::is_invocable_r_v<bool, PRED, ObjectType const&>, bool> = true
#define __oP_cHeCkEr std::enable_if_t<std::is_invocable_v<OP, ObjectType&> || std::is_invocable_v<OP, ObjectType&, SizeType>, bool> = true

constexpr auto DEFAULT_LESS_THAN    = [](auto&& l, auto && r) -> bool { return l < r; };
constexpr auto DEFAULT_GREATER_THAN = [](auto&& l, auto && r) -> bool { return r < l; };

#endif //CUB_ARRAY_PREDCHECKER_H
