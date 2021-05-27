//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ObjectArray.h>
#include <catch.hpp>


SCENARIO("ObjectArray") {
    ObjectArray<int, 10> array;

    static_assert(std::is_trivially_destructible_v<ObjectArray<int, 10>>);

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

    WHEN("copy") {
        auto array2 = std::move(array);
        REQUIRE(array2.GetNum() == 1);
        REQUIRE(array.GetNum() == 0);
    }

}

namespace {
    int object_count = 0;

    struct Foo {
        int a;
        Foo(int a) : a{a} { object_count++; }
        ~Foo() { object_count--; }
    };

    static_assert(!std::is_trivially_destructible_v<ObjectArray<Foo, 10>>);
}

SCENARIO("ObjectArray Cleanup") {
    {
        ObjectArray<Foo, 10> array;
        REQUIRE(object_count == 0);
        array.Append(1);
        REQUIRE(object_count == 1);
        array.Append(2);
        REQUIRE(object_count == 2);
    }
    REQUIRE(object_count == 0);
}

SCENARIO("ObjectArray equality") {
    ObjectArray<int, 10> array1;
    array1.Append(0);
    array1.Append(1);

    {
        ObjectArray<int, 10> array2;
        array2.Append(1);
        array2.Append(0);
        REQUIRE(array1 == array2);
        array2.Append(0);
        REQUIRE(array1 != array2);
    }

    {
        ObjectArray<int, 10> array2;
        array2.Append(1);
        array2.Append(2);
        REQUIRE(array1 != array2);
    }
}