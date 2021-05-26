//
// Created by Darwin Yuan on 2021/5/27.
//

#include <cub/array/OrderedObjectArray.h>
#include <catch.hpp>

SCENARIO("OrderedObjectArray ASC") {
    OrderedObjectArray<int, 10, SortKind::EAGER_ASC> array;

    REQUIRE(array->GetNum() == 0);
    REQUIRE(array->IsEmpty());
    array.Append(10);
    array.Append(2);
    array.Append(7);
    REQUIRE(array->GetNum() == 3);
    REQUIRE(array[0] == 2);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 10);
}

SCENARIO("OrderedObjectArray DESC") {
    OrderedObjectArray<int, 10, SortKind::EAGER_DESC> array;

    REQUIRE(array->GetNum() == 0);
    REQUIRE(array->IsEmpty());
    array.Append(10);
    array.Append(2);
    array.Append(7);
    REQUIRE(array->GetNum() == 3);
    REQUIRE(array[0] == 10);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 2);
}