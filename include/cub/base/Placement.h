//
// Created by Darwin Yuan on 2021/5/25.
//

#ifndef CUB_ARRAY_PLACEMENT_H
#define CUB_ARRAY_PLACEMENT_H

#include <cub/base/detail/UnionTrait.h>
#include <utility>
#include <new>

template<typename T>
struct Placement
{
    Placement() = default;

    template<typename ... ARGS>
    auto Emplace(ARGS&& ... args) -> T* {
        if constexpr (std::is_aggregate_v<T>) {
            return new (&obj) T{ std::forward<ARGS>(args)... };
        } else {
            return new (&obj) T( std::forward<ARGS>(args)... );
        }
    }

    auto Destroy() -> void {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            GetObj()->~T();
        }
    }

    template<typename ... ARGS>
    auto Replace(ARGS&& ... args) -> T* {
        Destroy();
        return Emplace(std::forward<ARGS>(args)...);
    }

    auto GetObj() const -> T const* {
        return reinterpret_cast<T const*>(&obj);
    }

    auto GetObj() -> T* {
        return reinterpret_cast<T*>(&obj);
    }

    auto GetRef() const -> T const& {
        return *GetObj();
    }

    auto GetRef() -> T& {
        return *GetObj();
    }

    auto operator->() -> T* {
        return GetObj();
    }

    auto operator->() const -> T const* {
        return GetObj();
    }

    auto operator*() const -> T const& {
        return *GetObj();
    }

    auto operator*() -> T& {
        return *GetObj();
    }

private:
    using Storage = typename detail::UnionTrait<T>::Type;
    Storage obj;
};

#endif //CUB_ARRAY_PLACEMENT_H
