#ifndef FORWARD_ITERATOR_TEMPLATE_H_GUARD
#define FORWARD_ITERATOR_TEMPLATE_H_GUARD

#include "iterators_general.h"

namespace data_struct
{
    template <typename T, typename Impl, typename Mut>
    class ForwardIterator
    {
        using Self = ForwardIterator;
        using Types = IterTraits<Self>;

        template <typename Mut_>
        using EnableIfMutable = std::enable_if_t<std::is_same_v<Mut_, Mutable_tag>>;

        template <typename T1>
        using EnableIfConvertible = std::enable_if_t<std::is_convertible_v<T1, Impl>>;

        friend typename Impl::Container;
        friend ForwardIterator<T, Impl, Const_tag>;

    public:
        using iterator_category = typename Types::Category;
        using difference_type   = typename Types::Difference;

        using value_type = typename Types::Value;
        using reference  = typename Types::Reference;
        using pointer    = typename Types::Pointer;

    public:
        ForwardIterator() = default;
        ~ForwardIterator() noexcept = default;

        ForwardIterator (Impl impl_) 
            : impl (impl_)
        {}

        template <typename T1, typename = EnableIfConvertible<T1>>
        ForwardIterator (T1 t) 
            : impl (t)
        {}

        template <typename Mut_, typename = EnableIfMutable<Mut_>>
        ForwardIterator (ForwardIterator<T, Impl, Mut_> const& it)  
            : impl (it.impl)
        {}

        friend
        bool operator== (Self const& lhs, Self const& rhs) {
            return lhs.impl.equal (rhs.impl);
        }

        friend
        bool operator!= (Self const& lhs, Self const& rhs) {
            return not (lhs == rhs);
        }

        reference operator*() const {
            return impl.get_value();
        }

        pointer operator->() const {
            return &operator*();
        }

        Self& operator++() {
            impl.next();
            return *this;
        }

        Self operator++ (int i) {
            auto tmp = *this;
            impl.next();
            return tmp;
        }

        decltype(auto) real() const {
            return impl.real();
        }

    protected:
        Impl impl{};
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