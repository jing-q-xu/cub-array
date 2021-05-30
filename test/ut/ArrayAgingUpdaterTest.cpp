//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ArrayAgingUpdater.h>
#include <cub/array/ArrayView.h>
#include <catch.hpp>

namespace {
    struct Phy {
        int id;
        int value;
    };

    struct Ind {
        Phy a[5];
        uint8_t num;
    };

    struct Local {
        int id;
        int value;

        auto operator<(Local const& rhs) const -> bool {
            return value < rhs.value;
        }
    };

    struct Temp {
        int id;
        int value;

        auto operator<(Temp const& rhs) const -> bool {
            return value < rhs.value;
        }
    };

    struct Policy {
        using FromArray = ArrayView<Phy, 5>;
        using ToArray = ObjectArray<Local, 6>;
        using BitMap = typename ToArray::BitMap;

        using TempObject = Temp;
        using TempArray = ObjectArray<TempObject, FromArray::MAX_SIZE>;

        auto Matches(Phy const& phy, Local const& local) const -> bool {
            return phy.id == local.id;
        }

        auto Less(TempObject const& temp, Local const& local) const -> bool {
            return temp.value < local.value;
        }

        auto Update(Local& local, Phy const& phy) -> void {
            local.value += phy.value;
        }

        auto Update(Local& local) -> void {
            local.value += 1;
        }

        auto Append(ToArray& to, Phy const& phy) -> void
        {
            to.Append(phy.id, phy.value);
        }

        auto Append(TempArray& to, Phy const& phy) -> void
        {
            to.Append(phy.id, phy.value);
        }

        auto Append(ToArray& to, TempObject const& obj) -> void
        {
            to.Append(obj.id, obj.value);
        }

        auto Replace(ToArray& to, ToArray::SizeType index, TempObject const& obj) -> void
        {
            to.Replace(index, obj.id, obj.value);
        }

        auto GetReplaceable() const -> BitMap {
            BitMap map;
            map.set();
            return map;
        }

        auto OnReplaced(BitMap) -> void {

        }
    };
}

SCENARIO("ArrayAgingUpdater") {

    ObjectArray<Local, 6> array;
    Policy policy;
    ArrayAgingUpdater update{array,  policy};

    {
        Ind ind{{{1, 2},{2,3},{3,4}, {4, 5}, {5, 6}}, 5};
        update(ArrayView{ind.a, ind.num});
        REQUIRE(array.GetNum() == 5);
        REQUIRE(array[0].value == 2);
        REQUIRE(array[0].id == 1);
    }

    {
        Ind ind{{{1, 2},{2,3},{7,4}, {8, 10}, {9, 5}}, 5};
        update(ArrayView{ind.a, ind.num});
        REQUIRE(array.GetNum() == 6);
        REQUIRE(array[0].id == 1);
        REQUIRE(array[1].id == 2);
        REQUIRE(array[2].id == 9);
        REQUIRE(array[3].id == 4);
        REQUIRE(array[4].id == 5);
        REQUIRE(array[5].id == 8);
    }
}