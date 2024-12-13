#ifndef RANDOM_ITERATOR_TEMPLATE_H_GUARD
#define RANDOM_ITERATOR_TEMPLATE_H_GUARD

#include "bidirectional_iterator.h"

namespace iter
{
    template <typename T, typename Impl, typename Mut>
    class RandomIterator
        : public BidirectIterator<T, Impl, Mut>
    {
        using Base = BidirectIterator<T, Impl, Mut>;
        using Self = RandomIterator;

        template <typename... Args>
        using EnableIfImpl = std::enable_if_t<
            IsConstructibleButNotBase_v <Impl, Args...>
        >;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;

        using typename Base::value_type;
        using typename Base::reference;
        using typename Base::pointer;

    public:
        using Base::operator*;
        using Base::operator->;
        using Base::operator++;
        using Base::operator--;

    public:
        RandomIterator() = default;

        template <typename... Args, typename = EnableIfImpl<Args...>>
        RandomIterator (Args&&... args)
            : Base (std::forward<Args> (args)...)
        {}

        template <typename Mut_>
        RandomIterator (RandomIterator<T, Impl, Mut_> const& it)  
            : Base ((Base const&) it)
        {}

        friend
        difference_type operator- (Self const& lhs, Self const& rhs) {
            return lhs.diff (rhs);
        }

        friend
        bool operator< (Self const& lhs, Self const& rhs) {
            return (lhs - rhs) < 0;
        }

        friend
        bool operator> (Self const& lhs, Self const& rhs) {
            return (lhs - rhs) > 0;
        }

        friend
        bool operator<= (Self const& lhs, Self const& rhs) {
            return lhs < rhs or lhs == rhs;
        }

        friend
        bool operator>= (Self const& lhs, Self const& rhs) {
            return lhs > rhs or lhs == rhs;
        }

        Self& operator+= (difference_type n) {
            Impl::plus (n);
            return *this;
        }

        Self& operator-= (difference_type n) {
            Impl::plus (-n);
            return *this;
        }

        friend
        Self operator+ (Self it, difference_type n) {
            return (it += n);
        }

        friend
        Self operator+ (difference_type n, Self it) {
            return it + n;
        }

        friend
        Self operator- (Self it, difference_type n) {
            return (it -= n);
        }

        reference operator[] (std::size_t ind) {
            return *(ind + *this);
        }
    };
}

#endif