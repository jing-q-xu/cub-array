//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_READONLYARRAYLIKE_H
#define CUB_ARRAY_READONLYARRAYLIKE_H

#include <cub/base/Placement.h>
#include <cub/base/DeduceSizeType.h>
#include <cub/base/SizeOfArray.h>
#include <cstdint>
#include <type_traits>
#include <bitset>
#include <optional>

template<typename OBJ>
struct ObjectTrait {
    using ElemType = OBJ;
    using ObjectType = OBJ;

    static auto ToObject(ElemType const& elem) -> ObjectType const& {
        return elem;
    }

    static auto ToObject(ElemType & elem) -> ObjectType& {
        return elem;
    }
};

template<typename T>
struct ObjectTrait<Placement<T>> {
    using ElemType = Placement<T>;
    using ObjectType = T;

    static auto ToObject(ElemType const& elem) -> ObjectType const& {
        return *elem;
    }

    static auto ToObject(ElemType & elem) -> ObjectType& {
        return *elem;
    }
};

template<typename DATA_HOLDER>
struct ReadOnlyArrayLike : private DATA_HOLDER {
    using ElemType = std::decay_t<decltype(*DATA_HOLDER::objs)>;

private:
    using Trait = ObjectTrait<ElemType>;
    using Data = DATA_HOLDER;

public:
    using ObjectType = typename Trait::ObjectType;

    constexpr static std::size_t MAX_SIZE = DATA_HOLDER::MAX_SIZE;
    static_assert(MAX_SIZE > 0);

    using SizeType   = DeduceSizeType_t<MAX_SIZE>;

    using BitMap = std::bitset<MAX_SIZE>;

private:
    template< typename OP>
    auto Visit(OP&& op, SizeType i) -> void {
        if constexpr (std::is_invocable_v<OP, ObjectType&, SizeType>) {
            op(Trait::ToObject(Data::objs[i]), i);
        } else {
            op(Trait::ToObject(Data::objs[i]));
        }
    }

    template< typename OP>
    auto Visit(OP&& op, SizeType i) const -> void {
        if constexpr (std::is_invocable_v<OP, ObjectType const&, SizeType>) {
            op(Trait::ToObject(Data::objs[i]), i);
        } else {
            op(Trait::ToObject(Data::objs[i]));
        }
    }

    template< typename PRED>
    auto Pred(PRED&& pred, SizeType i) const -> bool {
        if constexpr (std::is_invocable_r_v<bool, PRED, ObjectType const&, SizeType>) {
            return pred(Trait::ToObject(Data::objs[i]), i);
        } else {
            return pred(Trait::ToObject(Data::objs[i]));
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
        return index ? &Trait::ToObject(Data::objs[*index]) : nullptr;
    }

public:
    using DATA_HOLDER::DATA_HOLDER;

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
};

#endif //CUB_ARRAY_READONLYARRAYLIKE_H
