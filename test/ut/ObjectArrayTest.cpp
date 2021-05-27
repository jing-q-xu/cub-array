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
    struct Foo {
        int a;
        ~Foo() {}
    };

    static_assert(!std::is_trivially_destructible_v<ObjectArray<Foo, 10>>);
}