//
// Created by Darwin Yuan on 2021/5/26.
//

#include <cub/array/detail/ReadOnlyArrayLike.h>
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

    using IntArray = detail::ReadOnlyArrayLike<IntArrayHolder>;
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
    using PlacementArray = detail::ReadOnlyArrayLike<PlacementArrayHolder>;
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

    REQUIRE(array[0] == 1);
    REQUIRE(array[1] == 2);
    REQUIRE(array[2] == 3);
    REQUIRE(array.GetNum() == 3);
    REQUIRE(array.GetFreeNum() == 7);

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

//    std::bitset<256> bs;
//    bs.set(250);
//    std::bitset<256> bs1{1};
//
//    REQUIRE(bs == bs1);
}

SCENARIO("const ReadOnlyArrayLike Find") {
    const IntArray array = {1, 2, 3};

    {
        auto result = array.FindIndex([](auto&& item) {
            return item == 2;
        });
        REQUIRE(result);
        REQUIRE(*result == 1);
    }

    {
        auto result = array.FindIndex([](auto&& item) {
            return item == 2;
        }, 10);
        REQUIRE(!result);
    }

    {
        IntArray::BitMap enabled{};
        enabled.set(0);
        enabled.set(2);

        auto result = array.FindIndex([](auto&& item) {
            return item == 2;
        }, enabled);
        REQUIRE(!result);
    }

    {
        IntArray::BitMap enabled{};
        enabled.set(1);
        enabled.set(2);

        auto result = array.FindIndex([](auto&& item) {
            return item == 2;
        });
        REQUIRE(result);
        REQUIRE(*result == 1);
    }



}

SCENARIO("const Placement ReadOnlyArrayLike MinElem") {
    const PlacementArray array = {2, 4, 3, 1, 8};

    {
        auto result = array.MinElemIndex([](auto&& l, auto&& r) {
            return l < r;
        });

        REQUIRE(result);
        REQUIRE(*result == 3);
    }

    {
        IntArray::BitMap enabled{};
        enabled.set(1);
        enabled.set(2);
        enabled.set(4);

        auto result = array.MinElemIndex([](auto&& l, auto&& r) {
            return l < r;
        }, enabled);

        REQUIRE(result);
        REQUIRE(*result == 2);

        result = array.MaxElemIndex([](auto&& l, auto&& r) {
            return l < r;
        }, enabled);

        REQUIRE(result);
        REQUIRE(*result == 4);
    }
}
