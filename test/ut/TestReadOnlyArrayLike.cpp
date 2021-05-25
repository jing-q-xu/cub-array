//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/ReadOnlyArrayLike.h>
#include <catch.hpp>

namespace {
    struct IntArrayHolder {
        static constexpr auto MAX_SIZE = 10;
        int objs[MAX_SIZE];
        DeduceSizeType_t<MAX_SIZE> num{};

        IntArrayHolder(std::initializer_list<int> list) {
            for(auto elem : list) {
                objs[num++] = elem;
                if(num == MAX_SIZE) break;
            }
        }
    };

    using IntArray = ReadOnlyArrayLike<IntArrayHolder>;
}

SCENARIO("ReadOnlyArrayLike ForEach") {
    IntArray array = {1,2,3};

    array.ForEach([](int elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int const& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    {
        int n = 0;
        array.ForEach([&](int elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int& elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n);
        });
        REQUIRE(n == 3);
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n + 1);
        }, 1);

        REQUIRE(n == 2);
    }


    {
        IntArray::BitMap bitMap;
        bitMap.set(1);

        int n = 0;
        int value = 0;
        array.ForEach([&](int const& elem) {
            ++n;
            value = elem;
        }, bitMap);

        REQUIRE(n == 1);
    }


}

SCENARIO("const ReadOnlyArrayLike ForEach") {
    const IntArray array = {1,2,3};

    array.ForEach([](int elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int const& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    {
        int n = 0;
        array.ForEach([&](int elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n);
        });
        REQUIRE(n == 3);
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n + 1);
        }, 1);

        REQUIRE(n == 2);
    }

}

namespace {
    struct PlacementArrayHolder {
        static constexpr auto MAX_SIZE = 10;
        Placement<int> objs[MAX_SIZE];
        DeduceSizeType_t<MAX_SIZE> num{};

        PlacementArrayHolder(std::initializer_list<int> list) {
            for(auto elem : list) {
                objs[num++].Emplace(elem);
                if(num == MAX_SIZE) break;
            }
        }
    };
    using PlacementArray = ReadOnlyArrayLike<PlacementArrayHolder>;
}

SCENARIO("Placement ReadOnlyArrayLike ForEach") {
    PlacementArray array = {1,2,3};

    array.ForEach([](int elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int const& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    {
        int n = 0;
        array.ForEach([&](int elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int& elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n);
        });
        REQUIRE(n == 3);
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n + 1);
        }, 1);

        REQUIRE(n == 2);
    }

}

SCENARIO("const Placement ReadOnlyArrayLike ForEach") {
    const PlacementArray array = {1,2,3};

    array.ForEach([](int elem, int i) {
        REQUIRE(elem == (i+1));
    });

    array.ForEach([](int const& elem, int i) {
        REQUIRE(elem == (i+1));
    });

    {
        int n = 0;
        array.ForEach([&](int elem) {
            REQUIRE(elem == ++n);
        });
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n);
        });
        REQUIRE(n == 3);
    }

    {
        int n = 0;
        array.ForEach([&](int const& elem) {
            REQUIRE(elem == ++n + 1);
        }, 1);

        REQUIRE(n == 2);
    }

}