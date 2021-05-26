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
        using TempObject = long;
        using TempArray = ObjectArray<TempObject, 5>;
        using Array = ObjectArray<int, 10>;
        using SizeType = typename Array::SizeType ;

        auto Matches(Phy const& phy, int local) const -> bool {
            return phy.a == local;
        }

        auto Less(TempObject const& l, TempObject const& r) -> bool {
            return l < r;
        }

        auto OnFound(int& local, Phy const& phy) -> void {
            local += phy.a;
        }

        auto OnNotFound(int& local) -> void {
            local += 1;
        }

        auto Append(Array& to, Phy const& phy) -> void
        {
            to.Append(phy.a);
        }

        auto Append(TempArray& to, Phy const& phy) -> void
        {
            to.Append(phy.a);
        }

        auto Append(Array& to, TempObject const& obj) -> void
        {
            to.Append((int)obj);
        }


        auto Replace(Array& to, SizeType index, TempObject const& obj) -> void
        {
            to.Replace(index, (int)obj);
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