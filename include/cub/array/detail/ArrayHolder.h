//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYHOLDER_H
#define CUB_ARRAY_ARRAYHOLDER_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/ObjectTrait.h>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace detail {
    template<typename T, std::size_t MAX_NUM, typename OBJ = T>
    struct ArrayHolder {
        static constexpr auto MAX_SIZE = MAX_NUM;
        using SizeType = DeduceSizeType_t<MAX_NUM>;
        using Trait = ObjectTrait<OBJ>;

        static_assert(sizeof(T) == sizeof(OBJ));
        static_assert(alignof(T) == alignof(OBJ));

    protected:
        auto ClearContent(SizeType from = 0) -> void {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for(int i=from; i<num; i++) Trait::Destroy(objs[i]);
            }
        }

        auto Clear(SizeType from = 0) -> void {
            ClearContent();
            num = std::min(from, num);
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

        auto MoveFrom(ArrayHolder&& rhs) {
            if constexpr (std::is_trivially_copyable_v<T>) {
                ::memcpy(objs, rhs.objs, sizeof(OBJ) * num);
            } else {
                for(int i=0; i<num; i++) {
                    Trait::Emplace(objs[i], std::move(Trait::ToObject(rhs.objs[i])));
                }
            }

            rhs.Clear();
        }

        auto CopyFrom(ArrayHolder const& rhs) {
            if constexpr (std::is_trivially_copyable_v<T>) {
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

    template<typename T, typename = void >
    struct ArrayHolderTrait {
        template<std::size_t MAX_NUM, typename OBJ = T>
        struct Type : ArrayHolder<T, MAX_NUM, OBJ> {
            using Parent = ArrayHolder<T, MAX_NUM, OBJ>;
            using Parent::Parent;

            ~Type() {
                Parent::ClearContent();
            }
        };
    };

    template<typename T>
    struct ArrayHolderTrait<T, std::enable_if_t<std::is_trivially_destructible_v<T>>> {
        template<std::size_t MAX_NUM, typename OBJ = T>
        struct Type : ArrayHolder<T, MAX_NUM, OBJ> {
            using Parent = ArrayHolder<T, MAX_NUM, OBJ>;
            using Parent::Parent;
        };
    };
}

#endif //CUB_ARRAY_ARRAYHOLDER_H
