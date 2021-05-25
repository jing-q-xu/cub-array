//
// Created by Darwin Yuan on 2021/5/26.
//

#include <catch.hpp>
#include <cub/base/Placement.h>

namespace {
    constexpr short DE_MAGIC = 0xE7A2;
    constexpr short CO_MAGIC = 0xABCD;
    static bool cons = false;

    struct Foo {
        Foo(long long a) : a(a), b{CO_MAGIC} {
            cons = true;
        }

        long long a;
        short b;

        ~Foo() {
            b = DE_MAGIC;
            cons = false;
        }
    };
}

SCENARIO("Placement") {
    Placement<Foo> obj;

    static_assert(alignof(obj) == alignof(Foo));

    REQUIRE(!cons);

    obj.Emplace(100);

    REQUIRE(cons);
    REQUIRE(obj->a == 100);
    REQUIRE((*obj).a == 100);
    REQUIRE(obj->b == CO_MAGIC);

    obj.Destroy();

    REQUIRE(!cons);
    REQUIRE(obj->b == DE_MAGIC);
}