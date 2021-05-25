//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_ARRAYVIEW_H
#define CUB_ARRAY_ARRAYVIEW_H

#include <cub/array/detail/ReadOnlyArrayLike.h>

namespace detail {
    template<typename T, std::size_t MAX_NUM>
    struct ArrayViewTrait
    {
    private:
        struct DataHolder {
            static constexpr auto MAX_SIZE = MAX_NUM;
            using SizeType = DeduceSizeType_t<MAX_NUM>;

            DataHolder(T const* objs, std::size_t num)
                : objs{objs}
                , num(std::min(num, MAX_NUM))
            {}

            T const* objs;
            SizeType num;
        };

        using Base = ReadOnlyArrayLike<DataHolder>;

    public:
        struct Type : Base {
            using Base::Base;
        };
    };
}

template<typename T, std::size_t MAX_NUM>
using ArrayView = typename detail::ArrayViewTrait<T, MAX_NUM>::Type;


#endif //CUB_ARRAY_ARRAYVIEW_H
