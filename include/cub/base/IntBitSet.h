//
// Created by Darwin Yuan on 2021/5/27.
//

#ifndef CUB_ARRAY_INTBITSET_H
#define CUB_ARRAY_INTBITSET_H

#include <cstdint>
#include <cstddef>

namespace detail {
    template<std::size_t N>
    auto DeduceBitSetIntType() -> auto {
        static_assert(N <= sizeof(uint64_t) * 8, "too big for int, try std::bitset");
        if constexpr (N <= sizeof(uint8_t) * 8) {
            return uint8_t{};
        } else if constexpr (N <= sizeof(uint16_t) * 8) {
            return uint16_t{};
        } else if constexpr (N <= sizeof(uint32_t) * 8) {
            return uint32_t{};
        } else {
            return uint64_t{};
        }
    }

    template<std::size_t N>
    using DeduceBitSetIntType_t = decltype(DeduceBitSetIntType<N>());

}

template<std::size_t N>
struct IntBitSet {
private:
    static_assert(N > 0);
    using IntType = detail::DeduceBitSetIntType_t<N>;
    constexpr static IntType MASK = (((IntType)1 << N) - 1);

public:
    constexpr IntBitSet() = default;
    constexpr IntBitSet(IntType integral) : integral(MASK & integral) {}

    constexpr auto operator==(IntBitSet const& rhs) const -> bool {
        return integral == rhs.integral;
    }

    constexpr auto operator!=(IntBitSet const& rhs) const -> bool {
        return integral != rhs.integral;
    }

    constexpr auto test(std::size_t pos) const -> bool {
        return (integral & (1ULL << pos)) > 0;
    }

    constexpr auto operator[](std::size_t pos) const -> bool {
        return test(pos);
    }

    constexpr auto all() const -> bool {
        return (integral & MASK) == MASK;
    }

    constexpr auto any() const -> bool {
        return integral > 0;
    }

    constexpr auto none() const -> bool {
        return integral == 0;
    }

    constexpr auto count() const -> std::size_t {
        std::size_t c = 0;
        for(auto n = integral; n; n &= (n - 1), c++);
        return c;
    }

    constexpr auto size() const -> std::size_t {
        return N;
    }

    auto set(std::size_t pos) -> void {
        integral |= (((IntType)1 << pos) & MASK);
    }

    auto set() -> void {
        integral = MASK;
    }

    auto set(std::size_t pos, bool value) -> void {
        value ? set(pos) : reset(pos);
    }

    auto reset(std::size_t pos) -> void {
        integral &= (~((IntType)1 << pos) & MASK);
    }

    auto reset() -> void {
        integral = 0;
    }

    auto flip() -> void {
        integral = (~integral & MASK);
    }

    auto operator&=(IntBitSet const& rhs) -> IntBitSet& {
        integral &= rhs.integral;
        return *this;
    }

    auto operator|=(IntBitSet const& rhs) -> IntBitSet& {
        integral |= rhs.integral;
        return *this;
    }

    auto operator^=(IntBitSet const& rhs) -> IntBitSet& {
        integral ^= rhs.integral;
        return *this;
    }

    auto operator<<(std::size_t pos) -> IntBitSet {
        return IntBitSet{integral << pos};
    }

    auto operator<<=(std::size_t pos) -> IntBitSet& {
        integral <<= pos;
        integral &= MASK;
        return *this;
    }

    auto operator>>(std::size_t pos) -> IntBitSet {
        return IntBitSet{integral << pos};
    }

    auto operator>>=(std::size_t pos) -> IntBitSet& {
        integral >>= pos;
        return *this;
    }

    constexpr auto operator~() const -> IntBitSet {
        return IntBitSet(~integral);
    }

    friend constexpr auto operator&(IntBitSet const& lhs, IntBitSet const rhs) -> IntBitSet {
        return IntBitSet(lhs.integral & rhs.integral);
    }

    friend constexpr auto operator|(IntBitSet const& lhs, IntBitSet const rhs) -> IntBitSet {
        return IntBitSet(lhs.integral | rhs.integral);
    }

    friend constexpr auto operator^(IntBitSet const& lhs, IntBitSet const rhs) -> IntBitSet {
        return IntBitSet(lhs.integral ^ rhs.integral);
    }

private:
    IntType integral{};
};

#endif //CUB_ARRAY_INTBITSET_H
