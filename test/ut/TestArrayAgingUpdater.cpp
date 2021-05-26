//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ArrayAgingUpdater.h>
#include <cub/array/ArrayView.h>
#include <catch.hpp>

namespace {
    struct Phy {
        int a;
        int b;
    };
    struct Ind {
        Phy a[5];
        uint8_t num;
    };

    struct Policy {
        using Array = ObjectArray<int, 10>;
        using SizeType = typename Array::SizeType ;

        auto Matches(Phy const& phy, int local) const -> bool {
            return phy.a == local;
        }

        using TEMP_OBJECT = long;

        auto Less(TEMP_OBJECT const&, TEMP_OBJECT const& ) -> bool {
            return false;
        }

        auto OnFound(int local) -> void {

        }

        auto OnNotFound(int local) -> void {

        }

        auto Append(Array& to, Phy const&) -> void
        {

        }

        auto Append(Array& to, long const&) -> void
        {

        }

        auto Append(ObjectArray<TEMP_OBJECT, 5>& to, Phy const&) -> void
        {

        }

        auto Replace(Array& to, SizeType index, long const&) -> void
        {

        }

        auto GetRemovable() const -> std::bitset<10> {
            return 0;
        }

        auto OnRemoved(std::bitset<10>) -> void {

        }
    };
}

SCENARIO("ArrayAgingUpdater") {
    Ind ind{{{1, 2},{2,3},{3,4}, {4, 5}}, 4};
    ObjectArray<int, 10> array;
    ArrayView view{ind.a, ind.num};
    Policy policy;
    ArrayAgingUpdater updater{view, array,  policy};

    updater();
}