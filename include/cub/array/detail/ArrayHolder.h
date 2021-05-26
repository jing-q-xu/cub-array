//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYHOLDER_H
#define CUB_ARRAY_ARRAYHOLDER_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/ObjectTrait.h>
#include <cstdint>

namespace detail {
    template<typename T, std::size_t MAX_NUM, typename OBJ = T>
    struct ArrayHolder {
        static constexpr auto MAX_SIZE = MAX_NUM;
        using SizeType = DeduceSizeType_t<MAX_NUM>;
        using Trait = ObjectTrait<OBJ>;

        static_assert(sizeof(T) == sizeof(OBJ));
        static_assert(alignof(T) == alignof(OBJ));
        
    private:
        static constexpr auto COULD_COPY = std::is_trivially_copyable_v<T> &&
                           std::is_trivially_copy_assignable_v<T> &&
                           std::is_trivially_copy_constructible_v<T>;

        auto Clear() -> void {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for(int i=0; i<num; i++) Trait::Destroy(objs[i]);
            }
            num = 0;
        }

    public:
        ArrayHolder() = default;
        ArrayHolder(ArrayHolder const& rhs) : num{rhs.num} {
            CopyFrom(rhs);
        }

        ArrayHolder(ArrayHolder&& rhs) : num{rhs.num} {
            MoveFrom(std::move(rhs));
        }

        auto operator=(ArrayHolder const& rhs) -> ArrayHolder& {
            Clear();
            num = rhs.num;
            CopyFrom(rhs);
        }

        auto operator=(ArrayHolder&& rhs) -> ArrayHolder& {
            Clear();
            num = rhs.num;
            MoveFrom(std::move(rhs));
        }

        ~ArrayHolder() {
            Clear();
        }

        auto MoveFrom(ArrayHolder&& rhs) {
            if constexpr (std::is_trivially_move_assignable_v<T> && std::is_trivially_move_constructible_v<T> && COULD_COPY) {
                ::memcpy(objs, rhs.objs, sizeof(OBJ) * num);
            } else {
                for(int i=0; i<num; i++) {
                    Trait::Emplace(objs[i], std::move(Trait::ToObject(rhs.objs[i])));
                }
            }

            rhs.Clear();
        }

        auto CopyFrom(ArrayHolder const& rhs) {
            if constexpr (COULD_COPY) {
                ::memcpy(objs, rhs.objs, sizeof(T) * num);
            } else {
                for(int i=0; i<num; i++) {
                    ObjectTrait<OBJ>::Emplace(objs[i], Trait::ToObject(rhs.objs[i]));
                }
            }
        }

    public:
        OBJ objs[MAX_SIZE];
        SizeType num{};
    };
}

#endif //CUB_ARRAY_ARRAYHOLDER_H
