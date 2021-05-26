//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ArraySortObject.h>
#include <cub/array/ArrayView.h>
#include <catch.hpp>

namespace {
    struct Dummy {
        int a[10];
        uint8_t num;
    };
}
SCENARIO("ArraySortObject") {
    Dummy dummy;
    dummy.num = 3;
    dummy.a[0] = 2;
    dummy.a[1] = 1;
    dummy.a[2] = 3;

    ArrayView<int, 10> array{dummy.a, dummy.num};

    ArraySortObject sortObject{array};

    auto cnt = sortObject.Sort([](auto&& l, auto&& r) {
       return l < r;
    });

    REQUIRE(cnt == 3);
}