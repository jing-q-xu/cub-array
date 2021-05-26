//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYAGINGUPDATER_H
#define CUB_ARRAY_ARRAYAGINGUPDATER_H

#include <cub/array/ArraySortObject.h>
#include <cub/array/ObjectArray.h>

template<typename POLICY, typename TO_ARRAY, typename FROM_ARRAY>
struct ArrayAgingUpdater {
    ArrayAgingUpdater(FROM_ARRAY const& from, TO_ARRAY& to, POLICY& policy)
            : from{from}, to{to}, policy{policy} {}

    auto operator()() -> void {
        Update();
        TryAppend();
    }

private:
    auto Update() -> void {
        from.ForEach([this](auto&& fromElem, auto i){
            auto index = to.FindIndex([&](auto&& toElem) {
                return policy.Matches(fromElem, toElem);
            });
            if(index) {
                policy.OnFound(to[*index]);
                updateFlag.set(i);
            } else {
                policy.OnNotFound(to[*index]);
            }
        });
    }

    auto TryAppend() -> void {
        UpdateFlag allUpdated{};
        allUpdated.flip();

        if(allUpdated == updateFlag) {
            return;
        }

        if(FROM_ARRAY::MAX_SIZE - updateFlag.count() < to.GetFreeNum()) {
            FastAppend();
        } else {
            SlowAppend();
        }
    }

    template<typename TARGET>
    auto AppendTo(TARGET&& target) {
        from.ForEach([&, this](auto&& fromElem, auto i) {
            if(!updateFlag.test(i)) {
                policy.Append(target, fromElem);
            }
        });
    }

    auto FastAppend() -> void {
        AppendTo(to);
    }

    auto SlowAppend() -> void {
        ObjectArray<typename POLICY::TEMP_OBJECT, FROM_ARRAY::MAX_SIZE> toBeAppended{};
        AppendTo(toBeAppended);
        ArraySortObject sorted{toBeAppended};
        auto&& less = [&](auto&& lhs, auto&& rhs) {
            return policy.Less(lhs, rhs);
        };
        auto cnt = sorted.Sort(less);

        auto rest = to.GetFreeNum();
        for(auto i=0; i<rest; i++) {
            policy.Append(to, sorted[i]);
        }

        std::bitset<TO_ARRAY::MAX_SIZE> removable = policy.GetRemovable();
        std::bitset<TO_ARRAY::MAX_SIZE> removed;
        for(auto i=rest; i<cnt; i++) {
            if(removable.none()) break;
            auto elemIndex = to.MinElemIndex(less, removable);
            if(!elemIndex) break;
            removable.reset(*elemIndex);
            removed.set(*elemIndex);
            policy.Replace(to, *elemIndex, sorted[i]);
        }

        if(removed.any()) {
            policy.OnRemoved(removed);
        }
    }

private:
    FROM_ARRAY const& from;
    TO_ARRAY&         to;
    POLICY&           policy;
    using UpdateFlag = std::bitset<FROM_ARRAY::MAX_SIZE>;
    UpdateFlag updateFlag{};
};

#endif //CUB_ARRAY_ARRAYAGINGUPDATER_H
