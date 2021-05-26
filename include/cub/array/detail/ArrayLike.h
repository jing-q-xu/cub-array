//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYLIKE_H
#define CUB_ARRAY_ARRAYLIKE_H

#include <cub/array/detail/ReadOnlyArrayLike.h>

namespace detail {
    template<typename DATA_HOLDER>
    struct ArrayLike : ReadOnlyArrayLike<DATA_HOLDER> {
    private:
        using Parent = ReadOnlyArrayLike<DATA_HOLDER>;
        using Trait = typename Parent::Trait;
        using Data = typename Parent::Data;

    public:
        using ObjectType = typename Parent::ObjectType;
        using SizeType = typename Parent::SizeType;
        constexpr static auto MAX_SIZE = Parent::MAX_SIZE;

    public:
        using Parent::Parent;

        template<typename ... ARGS>
        auto Append(ARGS &&... args) -> ObjectType * {
            if (Data::num >= MAX_SIZE) return nullptr;
            Trait::Emplace(Data::objs[Data::num], std::forward<ARGS>(args)...);
            return &(*this)[Data::num++];
        }

        template<typename ... ARGS>
        auto Replace(SizeType i, ARGS &&... args) -> ObjectType * {
            if (i >= MAX_SIZE) return nullptr;
            Trait::Replace(Data::objs[i], std::forward<ARGS>(args)...);
            return &(*this)[i];
        }

        auto Erase(SizeType i) -> void {
            if (i >= Data::num) return;
            if (i < Data::num - 1) {
                Trait::Replace(Data::objs[i], std::move((*this)[Data::num - 1]));
            }

            Trait::Destroy(Data::objs[--Data::num]);
        }

        auto Remove(ObjectType *p) -> void {
            auto base = &(*this)[0];
            if (p < base) return;
            Erase(p - base);
        }

        auto Clear() -> void {
            for (auto i = 0; i < Data::num; i++) {
                Trait::Destroy(Data::objs[i]);
            }
            Data::num = 0;
        }
    };
}

#endif //CUB_ARRAY_ARRAYLIKE_H
