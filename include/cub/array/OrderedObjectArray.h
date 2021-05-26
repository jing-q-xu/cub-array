//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ORDEREDOBJECTARRAY_H
#define CUB_ARRAY_ORDEREDOBJECTARRAY_H

#include <cub/array/OrderedArray.h>
#include <cub/array/ObjectArray.h>

enum class SortKind {
    NONE,
    EAGER_ASC,
    EAGER_DESC
};

template<typename OBJ, std::size_t MAX_SIZE, SortKind SORT_KIND = SortKind::NONE>
struct OrderedObjectArray : OrderedArray<ObjectArray<OBJ, MAX_SIZE>> {
    using Parent = OrderedArray<ObjectArray<OBJ, MAX_SIZE>>;
    using SizeType = typename Parent::SizeType;
    using ObjectType = OBJ;

private:

    auto TrySort() -> void {
        if constexpr (SORT_KIND == SortKind::EAGER_ASC) {
            Parent::Sort();
        } else if(SORT_KIND == SortKind::EAGER_DESC) {
            Parent::DescSort();
        }
    }

public:
    template<typename ... ARGS>
    auto Append(ARGS &&... args) -> auto {
        auto* result = Parent::GetArray().Append(std::forward<ARGS>(args)...);
        TrySort();
        return result;
    }

    template<typename ... ARGS>
    auto Replace(SizeType i, ARGS &&... args) -> auto {
        auto* result = Parent::GetArray().Replace(i, std::forward<ARGS>(args)...);
        TrySort();
        return result;
    }

    auto Erase(SizeType i) -> void {
        Parent::GetArray().Erase(i);
        TrySort();
    }

    auto Remove(ObjectType *p) -> void {
        Parent::GetArray().Remove(p);
        TrySort();
    }

    auto Clear() -> void {
        Parent::GetArray().Clear();
        TrySort();
    }
};

#endif //CUB_ARRAY_ORDEREDOBJECTARRAY_H
