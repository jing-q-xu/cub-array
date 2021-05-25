//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_READONLYARRAYLIKE_H
#define CUB_ARRAY_READONLYARRAYLIKE_H

#include <cub/array/detail/ObjectTrait.h>
#include <cub/base/DeduceSizeType.h>
#include <cub/base/SizeOfArray.h>
#include <cstdint>
#include <type_traits>
#include <bitset>
#include <optional>

template<typename DATA_HOLDER>
struct ReadOnlyArrayLike : protected DATA_HOLDER {
    using ElemType = std::decay_t<decltype(*DATA_HOLDER::objs)>;

protected:
    using Trait = detail::ObjectTrait<ElemType>;
    using Data = DATA_HOLDER;

public:
    using ObjectType = typename Trait::ObjectType;

    constexpr static std::size_t MAX_SIZE = DATA_HOLDER::MAX_SIZE;
    static_assert(MAX_SIZE > 0);

    using SizeType   = DeduceSizeType_t<MAX_SIZE>;

    using BitMap = std::bitset<MAX_SIZE>;

public:
    using DATA_HOLDER::DATA_HOLDER;

    auto GetNum() const -> SizeType {
        return Data::num;
    }

    auto IsEmpty() const -> bool {
        return Data::num == 0;
    }

    auto IsFull() const -> bool {
        return Data::num == MAX_SIZE;
    }

    auto GetFreeNum() const -> SizeType {
        return MAX_SIZE - Data::num;
    }

    auto operator[](SizeType n) const -> decltype(auto) {
        return (Trait::ToObject(Data::objs[n]));
    }

    auto operator[](SizeType n) -> decltype(auto) {
        return (Trait::ToObject(Data::objs[n]));
    }

private:
    template< typename OP>
    auto Visit(OP&& op, SizeType i) -> void {
        if constexpr (std::is_invocable_v<OP, ObjectType&, SizeType>) {
            op((*this)[i], i);
        } else {
            op((*this)[i]);
        }
    }

    template< typename OP>
    auto Visit(OP&& op, SizeType i) const -> void {
        if constexpr (std::is_invocable_v<OP, ObjectType const&, SizeType>) {
            op((*this)[i], i);
        } else {
            op((*this)[i]);
        }
    }

    template< typename PRED>
    auto Pred(PRED&& pred, SizeType i) const -> bool {
        if constexpr (std::is_invocable_r_v<bool, PRED, ObjectType const&, SizeType>) {
            return pred((*this)[i], i);
        } else {
            return pred((*this)[i]);
        }
    }

    auto GetRemain(BitMap enabled, SizeType from) const -> BitMap {
        if constexpr (MAX_SIZE <= sizeof(unsigned long long)) {
            return enabled & BitMap{~((1ULL << from) - 1)};
        } else {
            BitMap remain = enabled;
            for(auto i=0; i<from; i++) {
                remain.reset(i);
                if(remain.none()) break;
            }
            return remain;
        }

    }

    auto GetObj(std::optional<SizeType> index) const -> ObjectType const*{
        return index ? &(*this)[*index] : nullptr;
    }

public:
    template< typename OP>
    auto ForEach(OP&& op, std::size_t from = 0) {
        for(auto i=from; i<Data::num; i++) {
            Visit(std::forward<OP>(op), i);
        }
    }

    template< typename OP>
    auto ForEach(OP&& op, BitMap enabled, std::size_t from = 0)
    {
        for(auto i=from; i<Data::num; i++) {
            if(enabled.test(i)) {
                Visit(std::forward<OP>(op), i);
            }
        }
    }

    template< typename OP>
    auto ForEach(OP&& op, std::size_t from = 0) const {
        for(auto i=from; i<Data::num; i++) {
            Visit(std::forward<OP>(op), i);
        }
    }

    template< typename OP>
    auto ForEach(OP&& op, BitMap enabled, std::size_t from = 0) const {
        for(auto i=from; i<Data::num; i++) {
            if(enabled.test(i)) {
                Visit(std::forward<OP>(op), i);
            }
        }
    }

    template<typename PRED>
    auto FindIndex(PRED&& pred, std::size_t from = 0) const -> std::optional<SizeType>
    {
        for(auto i=from; i<Data::num; i++) {
            if(Pred(std::forward<PRED>(pred), i)) {
                return i;
            }
        }
        return std::nullopt;
    }

    template<typename PRED>
    auto Find(PRED&& pred, SizeType from = 0) const -> ObjectType const*
    {
        return GetObj(FindIndex(pred, from));
    }

    template<typename PRED>
    auto Find(PRED&& pred, SizeType from = 0) -> ObjectType*
    {
        return const_cast<ObjectType*>(GetObj(FindIndex(pred, from)));
    }

    template<typename PRED>
    auto FindIndex(PRED&& pred, BitMap enabled, SizeType from = 0) const -> std::optional<SizeType>
    {
        from = std::min(from, Data::num);
        auto remain = from == 0 ? enabled : GetRemain(enabled, from);
        for(auto i=from; i<Data::num; i++) {
            if(remain.none()) break;

            if(remain.test(i) && Pred(std::forward<PRED>(pred), i)) {
                return i;
            }

            remain.reset(i);
        }
        return std::nullopt;
    }

    template<typename PRED>
    auto Find(PRED&& pred, BitMap enabled, SizeType from = 0) const -> ObjectType const*
    {
        return GetObj(FindIndex(pred, enabled, from));
    }

    template<typename PRED>
    auto Find(PRED&& pred, BitMap enabled, SizeType from = 0) -> ObjectType*
    {
        return const_cast<ObjectType*>(GetObj(FindIndex(pred, enabled, from)));
    }

    template<typename LESS>
    auto MinElem(LESS&& less, SizeType from = 0) const -> ObjectType const*
    {
        if(Data::num <= from) return nullptr;
        auto found = std::min_element(Data::objs + from, Data::objs + Data::num, [&](auto&& l, auto&& r){
            return less(Trait::ToObject(l), Trait::ToObject(r));
        });
        return (found == Data::objs + Data::num) ? nullptr : &Trait::ToObject(*found);
    }

    template<typename LESS>
    auto MinElem(LESS&& less, BitMap enabled, SizeType from = 0) const -> ObjectType const*
    {
        SizeType indices[MAX_SIZE];
        auto n = 0;
        for(auto i=from; i<Data::num; i++) {
            if(enabled.test(i)) {
                indices[n++] = i;
            }
        }
        if(n == 0) return nullptr;

        auto found = std::min_element(indices, indices + n, [&](auto&& l, auto&& r){
            return less((*this)[l], (*this)[r]);
        });
        return (found == indices + n) ? nullptr : &(*this)[*found];
    }

    template<typename LESS>
    auto MinElem(LESS&& less, BitMap enabled, SizeType from = 0) -> ObjectType*
    {
        return const_cast<ObjectType*>(const_cast<ReadOnlyArrayLike const*>(this)->MinElem(std::forward<LESS>(less), enabled, from));
    }

    template<typename LESS>
    auto MinElemIndex(LESS&& less, BitMap enabled, SizeType from = 0) const -> std::optional<SizeType>
    {
        auto found = MinElem(std::forward<LESS>(less), enabled, from);
        return (found == nullptr) ? std::nullopt : std::optional{found - &(*this)[0]};
    }

    template<typename LESS>
    auto MinElem(LESS&& less, SizeType from = 0) -> ObjectType*
    {
        return const_cast<ObjectType*>(const_cast<ReadOnlyArrayLike const*>(this)->MinElem(std::forward<LESS>(less), from));
    }

    template<typename LESS>
    auto MinElemIndex(LESS&& less, SizeType from = 0) const -> std::optional<SizeType>
    {
        auto found = MinElem(std::forward<LESS>(less), from);
        return (found == nullptr) ? std::nullopt : std::optional{found - &(*this)[0]};
    }

    template<typename LESS>
    auto MaxElem(LESS&& less, SizeType from = 0) const -> ObjectType const*
    {
        if(Data::num <= from) return nullptr;
        auto found = std::max_element(Data::objs + from, Data::objs + Data::num, [&](auto&& l, auto&& r){
            return less(Trait::ToObject(l), Trait::ToObject(r));
        });
        return (found == Data::objs + Data::num) ? nullptr : &Trait::ToObject(*found);
    }

    template<typename LESS>
    auto MaxElem(LESS&& less, SizeType from = 0) -> ObjectType*
    {
        return const_cast<ObjectType*>(const_cast<ReadOnlyArrayLike const*>(this)->MaxElem(std::forward<LESS>(less), from));
    }

    template<typename LESS>
    auto MaxElemIndex(LESS&& less, SizeType from = 0) const -> std::optional<SizeType>
    {
        auto found = MaxElem(std::forward<LESS>(less), from);
        return (found == nullptr) ? std::nullopt : std::optional{found - &(*this)[0]};
    }
};

#endif //CUB_ARRAY_READONLYARRAYLIKE_H
