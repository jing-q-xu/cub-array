//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ObjectArray.h>
#include <catch.hpp>

SCENARIO("ObjectArray") {
    ObjectArray<int, 10> array;

    REQUIRE(array.GetNum() == 0);
    REQUIRE(array.IsEmpty());
    array.Append(1);

    WHEN("Erase")
    {
        REQUIRE(array.GetNum() == 1);
        REQUIRE(!array.IsEmpty());
        array.Erase(0);
        REQUIRE(array.GetNum() == 0);
        REQUIRE(array.IsEmpty());
    }

    WHEN("Remove")
    {
        REQUIRE(array.GetNum() == 1);
        REQUIRE(!array.IsEmpty());

        array.Remove(&array[0]);

        REQUIRE(array.GetNum() == 0);
        REQUIRE(array.IsEmpty());
    }

}