//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/OrderedArray.h>
#include <cub/array/ObjectArray.h>
#include <catch.hpp>

SCENARIO("OrderedArray") {
    OrderedArray<ObjectArray<int, 10>> array;

    REQUIRE(array->GetNum() == 0);
    REQUIRE(array->IsEmpty());
    array->Append(10);
    array->Append(2);
    array->Append(7);
    REQUIRE(array->GetNum() == 3);
    REQUIRE(3 == array.Sort());

    REQUIRE(array[0] == 2);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 10);

    REQUIRE(3 == array.DescSort());

    REQUIRE(array[2] == 2);
    REQUIRE(array[1] == 7);
    REQUIRE(array[0] == 10);
}