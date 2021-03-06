//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_OBJECTTRAIT_H
#define CUB_ARRAY_OBJECTTRAIT_H

#include <cub/base/Placement.h>
#include <utility>

namespace detail {
    template<typename OBJ>
    struct ObjectTrait {
        using ElemType = OBJ;
        using ObjectType = OBJ;

        static auto ToObject(ElemType const& elem) -> ObjectType const& {
            return elem;
        }

        static auto ToObject(ElemType & elem) -> ObjectType& {
            return elem;
        }

        template<typename ... ARGS>
        static auto Emplace(ElemType & elem, ARGS&& ... args) -> void {
            if constexpr (std::is_aggregate_v<ElemType>) {
                elem = ObjectType{std::forward<ARGS>(args)...};
            } else {
                elem = ObjectType(std::forward<ARGS>(args)...);
            }
        }

        static auto Destroy(ElemType & elem) -> void {
            if constexpr (!std::is_trivially_destructible_v<ElemType>) {
                elem.~ElemType();
            }
        }

        template<typename ... ARGS>
        static auto Replace(ElemType & elem, ARGS&& ... args) -> void {
            Destroy(elem);
            Emplace(elem, std::forward<ARGS>(args) ...);
        }
    };

    template<typename T>
    struct ObjectTrait<Placement<T>> {
        using ElemType = Placement<T>;
        using ObjectType = T;

        static auto ToObject(ElemType const& elem) -> ObjectType const& {
            return *elem;
        }

        static auto ToObject(ElemType & elem) -> ObjectType& {
            return *elem;
        }

        template<typename ... ARGS>
        static auto Emplace(ElemType & elem, ARGS&& ... args) -> void {
            elem.template Emplace(std::forward<ARGS>(args)...);
        }

        template<typename ... ARGS>
        static auto Replace(ElemType & elem, ARGS&& ... args) -> void {
            elem.template Replace(std::forward<ARGS>(args)...);
        }

        static auto Destroy(ElemType & elem) -> void {
            elem.Destroy();
        }
    };
}

#endif //CUB_ARRAY_OBJECTTRAIT_H
