#ifndef FORWARD_ITERATOR_TEMPLATE_H_GUARD
#define FORWARD_ITERATOR_TEMPLATE_H_GUARD

#include "iterator_traits.h"

namespace iter
{
    template <typename T, typename Impl, typename Mut>
    class ForwardIterator : public Impl
    {
        using Self = ForwardIterator;
        using Types = IterTraits<Self>;

        friend ForwardIterator<T, Impl, Const_tag>;

        template <typename Mut_>
        using EnableIfMutable = std::enable_if_t<
            std::is_same_v<Mut_, Mutable_tag>
        >;

        template <typename... Args>
        using EnableIfImpl = std::enable_if_t<
            IsConstructibleButNotBase_v <Impl, Args...>
        >;

    public:
        using iterator_category = typename Types::Category;
        using difference_type   = typename Types::Difference;

        using value_type = typename Types::Value;
        using reference  = typename Types::Reference;
        using pointer    = typename Types::Pointer;

    public:
        ForwardIterator() = default;

        template <typename... Args, typename = EnableIfImpl<Args...>>
        ForwardIterator (Args&&... args)
            : Impl (std::forward<Args> (args)...)
        {}

        template <typename Mut_, typename = EnableIfMutable<Mut_>>
        ForwardIterator (ForwardIterator<T, Impl, Mut_> const& it)  
            : Impl (it)
        {}

        friend
        bool operator== (Self const& lhs, Self const& rhs) {
            return lhs.equal (rhs);
        }

        friend
        bool operator!= (Self const& lhs, Self const& rhs) {
            return not (lhs == rhs);
        }

        reference operator*() const {
            return Impl::get_value();
        }

        pointer operator->() const {
            return &operator*();
        }

        Self& operator++() {
            Impl::next();
            return *this;
        }

        Self operator++ (int i) {
            auto tmp = *this;
            Impl::next();
            return tmp;
        }
    };


    template <typename T, typename Impl>
    struct IterTraits<
        ForwardIterator<T, Impl, Mutable_tag>
    > {
        using Category   = std::forward_iterator_tag;
        using Difference = void;

        using Value     = T;
        using Reference = T&;
        using Pointer   = T*;
    };


    template <typename T, typename Impl>
    struct IterTraits<
        ForwardIterator<T, Impl, Const_tag>
    > {
        using Category   = std::forward_iterator_tag;
        using Difference = void;

        using Value     = T;
        using Reference = T const&;
        using Pointer   = T const*;
    };
}

#endif