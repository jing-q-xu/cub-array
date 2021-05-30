//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYAGINGUPDATER_H
#define CUB_ARRAY_ARRAYAGINGUPDATER_H

#include <cub/array/ArraySortObject.h>
#include <cub/array/ObjectArray.h>
#include <cub/base/BitSet.h>

template<typename POLICY>
struct ArrayAgingUpdater {
    using FromArray = typename POLICY::FromArray;
    using ToArray = typename POLICY::ToArray;
    using TempObject = typename POLICY::TempObject;
    using ToObject = typename ToArray::ObjectType;

    ArrayAgingUpdater(ToArray& to, POLICY& policy)
            : to{to}, policy{policy} {}

    auto operator()(FromArray const& from) -> void {
        Update(from);
        TryAppend(from);
    }

private:
    auto Update(FromArray const& from) -> void {
        from.ForEach([this](auto&& fromElem, auto i){
            auto index = to.FindIndex([&](auto&& toElem) {
                return policy.Matches(fromElem, toElem);
            });
            if(index) {
                policy.Update(to[*index], fromElem);
                updateFlag.set(i);
            } else {
                policy.Update(to[*index]);
            }
        });
    }

    auto TryAppend(FromArray const& from) -> void {
        auto numOfUpdated = updateFlag.count();

        if(numOfUpdated == from.GetNum()) {
            return;
        }

        if(from.GetNum() - numOfUpdated <= to.GetFreeNum()) {
            FastAppend(from);
        } else {
            SlowAppend(from);
        }
    }

    template<typename TARGET>
    auto AppendTo(TARGET&& target, FromArray const& from) {
        from.ForEach([&, this](auto&& fromElem, auto i) {
            if(!updateFlag[i]) {
                policy.Append(target, fromElem);
            }
        });
    }

    auto FastAppend(FromArray const& from) -> void {
        AppendTo(to, from);
    }

    auto SlowAppend(FromArray const& from) -> void {
        ObjectArray<TempObject, FromArray::MAX_SIZE> toBeAppended{};
        AppendTo(toBeAppended, from);
        ArraySortObject sorted{toBeAppended};

        auto cnt = sorted.DescSort();
        auto rest = to.GetFreeNum();

        BitSet<ToArray::MAX_SIZE> removable = policy.GetRemovable();
        BitSet<ToArray::MAX_SIZE> removed;

        for(auto i=rest; i<cnt; i++) {
            if(removable.none()) break;
            auto elemIndex = to.MinElemIndex(removable);
            if(!elemIndex) break;

            if(policy.Less(sorted[i], to[*elemIndex])) break;

            removable.reset(*elemIndex);
            removed.set(*elemIndex);
            policy.Replace(to, *elemIndex, sorted[i]);
        }

        if(removed.any()) {
            policy.OnRemoved(removed);
        }

        for(auto i=0; i<rest; i++) {
            policy.Append(to, sorted[i]);
        }
    }

private:
    ToArray&         to;
    POLICY&          policy;
    using UpdateFlag = BitSet<FromArray::MAX_SIZE>;
    UpdateFlag updateFlag{};
};

#endif //CUB_ARRAY_ARRAYAGINGUPDATER_H
