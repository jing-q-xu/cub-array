//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYLIKE_H
#define CUB_ARRAY_ARRAYLIKE_H

#include <cub/array/detail/ReadOnlyArrayLike.h>
#include <cub/array/detail/LessChecker.h>

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
            --Data::num;
            if (i < Data::num) {
                Trait::Replace(Data::objs[i], std::move((*this)[Data::num]));
            }

            Trait::Destroy(Data::objs[Data::num]);
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

        template<typename LESS, __lEsS_cHeCkEr>
        auto Sort(LESS&& less) -> void {
            std::stable_sort(Data::objs, Data::objs + Data::num, std::forward<LESS>(less));
        }

        auto Sort() -> void {
            Sort(DEFAULT_LESS_THAN);
        }

        auto DescSort() -> void {
            Sort(DEFAULT_GREATER_THAN);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto PartialSort(LESS&& less, SizeType n) -> SizeType {
            if(n == 0) return 0;

            if(n < Data::num) {
                std::partial_sort(Data::objs, Data::objs + n, Data::objs + Data::num, std::forward<LESS>(less));
                return n;
            } else {
                Sort(std::forward<LESS>(less));
                return Data::num;
            }
        }

        auto PartialSort(SizeType n) -> SizeType {
            return PartialSort(DEFAULT_LESS_THAN, n);
        }

        auto PartialDescSort(SizeType n) -> SizeType {
            return PartialSort(DEFAULT_GREATER_THAN, n);
        }
    };
}

#endif //CUB_ARRAY_ARRAYLIKE_H
