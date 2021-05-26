//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYHOLDER_H
#define CUB_ARRAY_ARRAYHOLDER_H

#include <cub/base/DeduceSizeType.h>
#include <cub/array/detail/ObjectTrait.h>
#include <cstdint>

namespace detail {
    template<typename T, std::size_t MAX_NUM, typename OBJ = T>
    struct ArrayHolder {
        static constexpr auto MAX_SIZE = MAX_NUM;
        using SizeType = DeduceSizeType_t<MAX_NUM>;
        using Trait = ObjectTrait<OBJ>;

        ArrayHolder() = default;
        ArrayHolder(ArrayHolder const& rhs) : num{rhs.num} {
            for(int i=0; i<num; i++) {
                Trait::Emplace(objs[i], Trait::ToObject(rhs.objs[i]));
            }
        }

        ArrayHolder(ArrayHolder&& rhs) : num{rhs.num} {
            for(int i=0; i<num; i++) {
                Trait::Emplace(objs[i], std::move(Trait::ToObject(rhs.objs[i])));
            }
            rhs.Clear();
        }

        auto operator=(ArrayHolder const& rhs) -> ArrayHolder& {
            Clear();
            num = rhs.num;
            for(int i=0; i<num; i++) {
                ObjectTrait<OBJ>::Emplace(objs[i], Trait::ToObject(rhs.objs[i]));
            }
        }

        auto operator=(ArrayHolder&& rhs) -> ArrayHolder& {
            Clear();
            num = rhs.num;
            for(int i=0; i<num; i++) {
                Trait::Emplace(objs[i], std::move(Trait::ToObject(rhs.objs[i])));
            }
        }

        ~ArrayHolder() {
            Clear();
        }

    private:
        auto Clear() -> void {
            for(int i=0; i<num; i++) {
                Trait::Destroy(objs[i]);
            }
            num = 0;
        }

    public:
        OBJ objs[MAX_SIZE];
        SizeType num{};
    };
}

#endif //CUB_ARRAY_ARRAYHOLDER_H
