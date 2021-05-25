//
// Created by Darwin Yuan on 2021/5/25.
//

#ifndef CUB_ARRAY_PLACEMENT_H
#define CUB_ARRAY_PLACEMENT_H

#include <utility>
#include <new>

template<typename T>
struct Placement
{
    Placement() = default;

    template<typename ... ARGS>
    auto Emplace(ARGS&& ... args) -> T* {
        return new (obj) T{ std::forward<ARGS>(args)... };
    }

    template<typename ... ARGS>
    auto Replace(ARGS&& ... args) -> T* {
        Destroy();
        return Emplace(std::forward<ARGS>(args)...);
    }

    auto Destroy() -> void {
        GetObject()->~T();
    }

    auto GetObject() const -> T const* {
        return reinterpret_cast<T const*>(obj);
    }

    auto GetObject() -> T* {
        return reinterpret_cast<T*>(obj);
    }

    auto operator->() -> T* {
        return GetObject();
    }

    auto operator->() const -> T const* {
        return GetObject();
    }

    auto operator*() const -> T const& {
        return *GetObject();
    }

    auto operator*() -> T& {
        return *GetObject();
    }

private:
    alignas(alignof(T)) char obj[sizeof(T)];
};

#endif //CUB_ARRAY_PLACEMENT_H
