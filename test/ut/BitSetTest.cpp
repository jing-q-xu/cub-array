//
// Created by Darwin Yuan on 2021/5/27.
//

#include <cub/base/BitSet.h>
#include <limits>
#include <catch.hpp>

SCENARIO("BitSet Size") {
    static_assert(sizeof(BitSet<8>) == 1);
    static_assert(sizeof(BitSet<9>) == 2);
    static_assert(sizeof(BitSet<16>) == 2);
    static_assert(sizeof(BitSet<17>) == 4);
    static_assert(sizeof(BitSet<32>) == 4);
    static_assert(sizeof(BitSet<33>) == 8);
    static_assert(sizeof(BitSet<64>) == 8);
}

SCENARIO("BitSet Count") {
    REQUIRE(BitSet<8>{0xFF}.count() == 8);
    REQUIRE(BitSet<8>{0xFE}.count() == 7);
    REQUIRE(BitSet<8>{0x02}.count() == 1);
}

SCENARIO("BitSet Flip") {
    BitSet<8> bitSet{0xFF};

    REQUIRE(bitSet[0]);

    bitSet.flip();
    REQUIRE(bitSet == BitSet<8>{0});
    REQUIRE(bitSet.none());
    REQUIRE(!bitSet.any());

    REQUIRE(!bitSet[0]);

    bitSet.flip();
    REQUIRE(!bitSet.none());
    REQUIRE(bitSet.any());
    REQUIRE(bitSet.all());
    REQUIRE(bitSet[0]);

    bitSet = BitSet<8>{0xFE};
    REQUIRE(!bitSet.none());
    REQUIRE(bitSet.any());
    REQUIRE(!bitSet.all());
    REQUIRE(!bitSet[0]);
}

SCENARIO("BitSet set/reset") {
    BitSet<8> bitSet{};

    REQUIRE(!bitSet[1]);
    bitSet.set(1);
    REQUIRE(bitSet[1]);

    REQUIRE(bitSet == BitSet<8>{2});

    bitSet.set(9);
    REQUIRE(bitSet == BitSet<8>{2});
    REQUIRE(!bitSet[9]);

    bitSet.reset(1);
    REQUIRE(bitSet == BitSet<8>{0});

    BitSet<7> bs;

    bs.set(7);
    REQUIRE(bs == BitSet<7>{0});

    bs.set();
    REQUIRE(bs == BitSet<7>{0x7F});
    bs.reset();
    REQUIRE(bs == BitSet<7>{0});

    bs.set(1, true);
    REQUIRE(bs == BitSet<7>{2});
    bs.set(1, false);
    REQUIRE(bs == BitSet<7>{0});
}

SCENARIO("BitSet bi-op") {
    REQUIRE((BitSet<7>(0x2) | BitSet<7>(0x01)) == BitSet<7>(0x03));
    REQUIRE((BitSet<7>(0x2) & BitSet<7>(0x03)) == BitSet<7>(0x02));
    REQUIRE(~BitSet<7>(0x2) == BitSet<7>{0x7D});

    REQUIRE((BitSet<7>{} ^ BitSet<7>{}) == BitSet<7>{});
};