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

    REQUIRE(array[0] == 10);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 2);

    REQUIRE(1 == array.Sort(1));
    REQUIRE(array[0] == 2);

    REQUIRE(1 == array.DescSort(1));
    REQUIRE(array[0] == 10);

    decltype(array)::BitMap bitmap;
    bitmap.set(0);

    REQUIRE(1 == array.Sort(bitmap));
    REQUIRE(array[0] == 10);

    REQUIRE(1 == array.DescSort(bitmap));
    REQUIRE(array[0] == 10);

    bitmap.set(1);

    REQUIRE(2 == array.Sort(bitmap));
    REQUIRE(array[0] == 2);
    REQUIRE(array[1] == 10);

    REQUIRE(2 == array.DescSort(bitmap));
    REQUIRE(array[0] == 10);
    REQUIRE(array[1] == 2);

    bitmap.reset(1);
    bitmap.set(2);

    REQUIRE(2 == array.Sort(bitmap));
    REQUIRE(array[0] == 7);
    REQUIRE(array[1] == 10);

    REQUIRE(2 == array.DescSort(bitmap));
    REQUIRE(array[0] == 10);
    REQUIRE(array[1] == 7);

    bitmap.set(1);

    bitmap.count();

    REQUIRE(3 == array.Sort(bitmap));
    REQUIRE(array[0] == 2);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 10);

    REQUIRE(3 == array.DescSort(bitmap));
    REQUIRE(array[0] == 10);
    REQUIRE(array[1] == 7);
    REQUIRE(array[2] == 2);


}