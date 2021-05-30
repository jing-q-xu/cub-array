//
// Created by Darwin Yuan on 2021/5/26.
//

#ifndef CUB_ARRAY_READONLYARRAYLIKE_H
#define CUB_ARRAY_READONLYARRAYLIKE_H

#include <cub/array/detail/ObjectTrait.h>
#include <cub/base/DeduceSizeType.h>
#include <cub/base/SizeOfArray.h>
#include <cub/base/BitSet.h>
#include <cub/array/detail/PredChecker.h>
#include <cstdint>
#include <type_traits>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////
#define __vIsIt_ReSuLt_TyPe \
std::decay_t<decltype(this->Visit(std::declval<OP&&>(), std::declval<SizeType>()))>

////////////////////////////////////////////////////////////////////////////////////////
#define __vIsIt_CoDe_BlOcK__ \
if constexpr(std::is_same_v<bool, __vIsIt_ReSuLt_TyPe>) { \
    if(auto r = Visit(std::forward<OP>(op), i); !r) return r; \
} else if constexpr(std::is_convertible_v<std::size_t, __vIsIt_ReSuLt_TyPe>) { \
    if(auto r = Visit(std::forward<OP>(op), i); r != 0) return r; \
} else  { \
    Visit(std::forward<OP>(op), i); \
}

////////////////////////////////////////////////////////////////////////////////////////
#define __fOrEaCh_SuCcEsS_rEtUrN__ \
if constexpr(std::is_same_v<bool, __vIsIt_ReSuLt_TyPe>) { \
    return true; \
} else if constexpr(std::is_convertible_v<std::size_t, __vIsIt_ReSuLt_TyPe>) { \
    return 0; \
} else {}

////////////////////////////////////////////////////////////////////////////////////////
namespace detail {

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

        using SizeType = DeduceSizeType_t<MAX_SIZE>;
        using BitMap = BitSet<MAX_SIZE>;

    public:
        using DATA_HOLDER::DATA_HOLDER;

        auto operator==(ReadOnlyArrayLike const &rhs) const -> bool {
            if (Data::num != rhs.GetNum()) return false;

            for (auto i = 0; i < Data::num; i++) {
                if(!rhs.template Exists([&mine = (*this)[i]](auto &&elem) { return mine == elem; })) {
                    return false;
                }
            }

            return true;
        }

        auto operator!=(ReadOnlyArrayLike const &rhs) const -> bool {
            return !operator==(rhs);
        }

        auto GetNum() const -> SizeType {
            return Data::num;
        }

        auto IsEmpty() const -> bool {
            return Data::num == 0;
        }

        auto IsFull() const -> bool {
            return Data::num == MAX_SIZE;
        }

        auto Any() const -> bool {
            return Data::Num > 0;
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

        auto ByIndex(SizeType n) const -> ObjectType* {
            return n < Data::num ? const_cast<ObjectType*>(&Trait::ToObject(Data::objs[n])) : nullptr;
        }

    private:
        template<typename OP, __oP_cHeCkEr>
        auto Visit(OP &&op, SizeType i) -> auto {
            if constexpr (std::is_invocable_v<OP, ObjectType&, SizeType>) {
                return op((*this)[i], i);
            } else {
                return op((*this)[i]);
            }
        }

        template<typename OP, __oP_cHeCkEr>
        auto Visit(OP &&op, SizeType i) const -> auto {
            if constexpr (std::is_invocable_v<OP, ObjectType const &, SizeType>) {
                return op((*this)[i], i);
            } else {
                return op((*this)[i]);
            }
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Pred(PRED&& pred, SizeType i) const -> bool {
            if constexpr (std::is_invocable_r_v<bool, PRED, ObjectType const &, SizeType>) {
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
                for (auto i = 0; i < from; i++) {
                    remain.reset(i);
                    if (remain.none()) break;
                }
                return remain;
            }

        }

        auto GetObj(std::optional<SizeType> index) const -> auto {
            return index ? &(*this)[*index] : nullptr;
        }

        auto GetIndex(ObjectType const * p) const -> std::optional<SizeType> {
            return (p == nullptr) ? std::nullopt : std::optional{p - &(*this)[0]};
        }

        auto Ref() -> decltype(auto) {
            return (*const_cast<decltype(this) const>(this));
        }

        template<bool EXCLUDE_SCOPE, typename LESS, __lEsS_cHeCkEr>
        auto MinElemIndex(LESS &&less, BitMap scope, SizeType from = 0) const -> std::optional<SizeType> {
            if (from >= Data::num) return std::nullopt;
            if constexpr(scope.size() <= sizeof(std::size_t) * 8) {
                if(EXCLUDE_SCOPE) {
                    scope = ~scope;
                }
                auto n = scope.size() - Data::num;
                int i = from;
                auto bits = (scope << n) >> (n + from);
                for(; bits.any(); bits >>= 1, ++i) {
                    if (!bits[0]) continue;
                    break;
                }

                if(bits.none()) return std::nullopt;

                auto minElem = i;
                while(bits.any()) {
                    bits >>= 1, ++i;
                    if(bits[0] && less((*this)[i], (*this)[minElem])) minElem = i;
                }
                return minElem;
            } else {
                std::optional<SizeType> start;
                for (auto i = from; i < Data::num; i++) {
                    if (scope[i] == EXCLUDE_SCOPE) continue;
                    start = i;
                    break;
                }

                if(!start) return std::nullopt;

                auto minElem = *start;
                for (auto i = minElem+1; i < Data::num; i++) {
                    if (scope[i] == EXCLUDE_SCOPE) continue;
                    if(less((*this)[i], (*this)[minElem])) minElem = i;
                }
                return minElem;
            }
        }

    public:
        auto Last() const -> ObjectType* {
            return Data::num == 0 ? nullptr : const_cast<ObjectType*>(&(*this)[Data::num - 1]);
        }

        template<typename OP, __oP_cHeCkEr>
        auto ForEach(OP &&op, SizeType from = 0) -> auto {
            for (SizeType i = from; i < Data::num; i++) {
                __vIsIt_CoDe_BlOcK__
            }
            __fOrEaCh_SuCcEsS_rEtUrN__
        }

        template<typename OP, __oP_cHeCkEr>
        auto ForEach(OP &&op, BitMap enabled, SizeType from = 0) -> auto {
            for (auto i = from; i < Data::num; i++) {
                if (!enabled[i]) continue;
                __vIsIt_CoDe_BlOcK__
            }
            __fOrEaCh_SuCcEsS_rEtUrN__
        }

        template<typename OP, __oP_cHeCkEr>
        auto ForEach(OP &&op, SizeType from = 0) const -> auto {
            for (auto i = from; i < Data::num; i++) {
                __vIsIt_CoDe_BlOcK__
            }
            __fOrEaCh_SuCcEsS_rEtUrN__
        }

        template<typename OP, __oP_cHeCkEr>
        auto ForEach(OP &&op, BitMap scope, SizeType from = 0) const -> auto {
            for (auto i = from; i < Data::num; i++) {
                if (!scope[i]) continue;
                __vIsIt_CoDe_BlOcK__
            }
            __fOrEaCh_SuCcEsS_rEtUrN__
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto FindIndex(PRED &&pred, SizeType from = 0) const -> std::optional<SizeType> {
            for (auto i = from; i < Data::num; i++) {
                if (Pred(std::forward<PRED>(pred), i)) {
                    return i;
                }
            }
            return std::nullopt;
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Find(PRED &&pred, BitMap scope, SizeType from = 0) const -> auto {
            return GetObj(FindIndex(pred, scope, from));
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Find(PRED &&pred, BitMap scope, SizeType from = 0) -> auto {
            return const_cast<ObjectType *>(GetObj(FindIndex(pred, scope, from)));
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Find(PRED &&pred, SizeType from = 0) const -> auto {
            return GetObj(FindIndex(pred, from));
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Find(PRED &&pred, SizeType from = 0) -> auto {
            return const_cast<ObjectType *>(GetObj(FindIndex(pred, from)));
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Exists(PRED &&pred, SizeType from = 0) const -> bool {
            return Find(std::forward<PRED>(pred), from) != nullptr;
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto FindIndex(PRED &&pred, BitMap enabled, SizeType from = 0) const -> std::optional<SizeType> {
            from = std::min(from, Data::num);
            auto remain = from == 0 ? enabled : GetRemain(enabled, from);
            for (auto i = from; i < Data::num; i++) {
                if (remain.none()) break;
                if(!remain[i]) continue;
                if (Pred(std::forward<PRED>(pred), i)) {
                    return i;
                }
                remain.reset(i);
            }

            return std::nullopt;
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto FindIndexEx(PRED &&pred, BitMap disabled, SizeType from = 0) const -> std::optional<SizeType> {
            for (auto i = from; i < Data::num; i++) {
                if (!disabled[i] && Pred(std::forward<PRED>(pred), i)) {
                    return i;
                }
            }

            return std::nullopt;
        }

        template<typename PRED, __pReD_cHeCkEr>
        auto Exists(PRED &&pred, BitMap enabled, SizeType from = 0) const -> bool {
            return Find(std::forward<PRED>(pred), enabled, from) != nullptr;
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElem(LESS &&less, SizeType from = 0) const -> ObjectType const * {
            if (Data::num <= from) return nullptr;
            auto found = std::min_element(Data::objs + from, Data::objs + Data::num, [&](auto &&l, auto &&r) {
                return less(Trait::ToObject(l), Trait::ToObject(r));
            });
            return (found == Data::objs + Data::num) ? nullptr : &Trait::ToObject(*found);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElemIndex(LESS &&less, BitMap scope, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndex<false>(std::forward<LESS>(less), scope, from);
        }

        auto MinElemIndex(BitMap enabled, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndex(DEFAULT_LESS_THAN, enabled, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElemIndexEx(LESS &&less, BitMap excluded, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndex<true>(std::forward<LESS>(less), excluded, from);
        }

        auto MinElemIndexEx(BitMap excluded, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndexEx(DEFAULT_LESS_THAN, excluded, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElem(LESS &&less, BitMap scope, SizeType from = 0) const -> ObjectType const * {
            return GetObj(MinElemIndex(std::forward<LESS>(less), scope, from));
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElem(LESS &&less, BitMap scope, SizeType from = 0) -> ObjectType * {
            return GetObj(MinElemIndex(std::forward<LESS>(less), scope, from));
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElemEx(LESS &&less, BitMap excluded, SizeType from = 0) const -> ObjectType const * {
            return GetObj(MinElemIndexEx(std::forward<LESS>(less), excluded, from));
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElemEx(LESS &&less, BitMap excluded, SizeType from = 0) -> ObjectType * {
            return GetObj(MinElemIndexEx(std::forward<LESS>(less), excluded, from));
        }

        auto MinElem(BitMap enabled, SizeType from = 0) const -> ObjectType const * {
            return MinElem(DEFAULT_LESS_THAN, enabled, from);
        }

        auto MinElem(BitMap enabled, SizeType from = 0) -> ObjectType * {
            return MinElem(DEFAULT_LESS_THAN, enabled, from);
        }

        auto MinElemEx(BitMap excluded, SizeType from = 0) const -> ObjectType const * {
            return MinElemEx(DEFAULT_LESS_THAN, excluded, from);
        }

        auto MinElemEx(BitMap excluded, SizeType from = 0) -> ObjectType * {
            return MinElemEx(DEFAULT_LESS_THAN, excluded, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElem(LESS &&less, SizeType from = 0) -> ObjectType * {
            return const_cast<ObjectType *>(const_cast<ReadOnlyArrayLike const *>(this)->MinElem(
                    std::forward<LESS>(less), from));
        }

        auto MinElem(SizeType from = 0) -> ObjectType * {
            return MinElem(DEFAULT_LESS_THAN, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MinElemIndex(LESS &&less, SizeType from = 0) const -> std::optional<SizeType> {
            return GetIndex(MinElem(std::forward<LESS>(less), from));
        }

        auto MinElemIndex(SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndex(DEFAULT_LESS_THAN);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElem(LESS &&less, SizeType from = 0) const -> ObjectType const * {
            return MinElem([&](auto &&l, auto r) { return less(r, l); }, from);
        }

        auto MaxElem(SizeType from = 0) const -> ObjectType const * {
            return MaxElem(DEFAULT_LESS_THAN, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElem(LESS &&less, SizeType from = 0) -> ObjectType * {
            return const_cast<ObjectType *>(Ref().MaxElem(std::forward<LESS>(less), from));
        }

        auto MaxElem(SizeType from = 0) -> ObjectType * {
            return MaxElem(DEFAULT_LESS_THAN, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElemIndex(LESS &&less, BitMap excluded, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndex([&](auto &&l, auto r) { return less(r, l); }, excluded, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElemIndexEx(LESS &&less, BitMap excluded, SizeType from = 0) const -> std::optional<SizeType> {
            return MinElemIndexEx([&](auto &&l, auto r) { return less(r, l); }, excluded, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElem(LESS &&less, BitMap enabled, SizeType from = 0) const -> auto {
            return GetObj(MaxElemIndex(std::forward<LESS>(less), enabled, from));
        }

        auto MaxElem(BitMap enabled, SizeType from = 0) const -> auto {
            return MaxElem(DEFAULT_LESS_THAN, enabled, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElem(LESS &&less, BitMap enabled, SizeType from = 0) -> auto {
            return GetObj(MaxElemIndex(std::forward<LESS>(less), enabled, from));
        }

        auto MaxElem(BitMap enabled, SizeType from = 0) -> auto {
            return MaxElem(DEFAULT_LESS_THAN, enabled, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElemEx(LESS &&less, BitMap enabled, SizeType from = 0) const -> auto {
            return GetObj(MaxElemIndexEx(std::forward<LESS>(less), enabled, from));
        }

        auto MaxElemEx(BitMap enabled, SizeType from = 0) const -> auto {
            return MaxElemEx(DEFAULT_LESS_THAN, enabled, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElemEx(LESS &&less, BitMap enabled, SizeType from = 0) -> auto {
            return GetObj(MaxElemIndexEx(std::forward<LESS>(less), enabled, from));
        }

        auto MaxElemEx(BitMap enabled, SizeType from = 0) -> auto {
            return MaxElemEx(DEFAULT_LESS_THAN, enabled, from);
        }

        template<typename LESS, __lEsS_cHeCkEr>
        auto MaxElemIndex(LESS &&less, SizeType from = 0) const -> std::optional<SizeType> {
            return GetIndex(MaxElem(std::forward<LESS>(less), from));
        }

        auto MaxElemIndex(SizeType from = 0) const -> std::optional<SizeType> {
            return MaxElemIndex(DEFAULT_LESS_THAN, from);
        }
    };
}

#endif //CUB_ARRAY_READONLYARRAYLIKE_H
