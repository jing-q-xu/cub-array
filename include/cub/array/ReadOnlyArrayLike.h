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

public:
    using DATA_HOLDER::DATA_HOLDER;

    template< typename OP>
    auto ForEach(OP&& op, std::size_t from = 0) {
        for(auto i=from; i<Data::num; i++) {
            Visit(std::forward<OP>(op), i);
        }
    }

    template< typename OP>
    auto ForEach(OP&& op, BitMap enabled, std::size_t from = 0) {
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
};

#endif //CUB_ARRAY_READONLYARRAYLIKE_H
