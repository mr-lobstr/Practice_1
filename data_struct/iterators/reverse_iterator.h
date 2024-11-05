#ifndef REVERSE_ITERATOR_TEMPLATE_H_GUARD
#define REVERSE_ITERATOR_TEMPLATE_H_GUARD

#include "iterator_traits.h"
#include "../my_utility.h"

namespace iter
{
    template <typename Iter, typename = EnableIfBidirect<Iter>>
    class ReverseIterator
    {
        using Self = ReverseIterator;
        using Types = IterTraits<Iter>;

    public:
        using iterator_category = typename Types::Category;
        using difference_type   = typename Types::Difference;

        using value_type = typename Types::Value;
        using reference  = typename Types::Reference;
        using pointer    = typename Types::Pointer;

    public:
        ReverseIterator() = default;

        
        explicit ReverseIterator (Iter iter_)
            : iter (iter_)
        {}

        friend
        bool operator== (Self const& lhs, Self const& rhs) {
            return lhs.iter == rhs.iter;
        }

        friend
        bool operator!= (Self const& lhs, Self const& rhs) {
            return not (lhs == rhs);
        }

        reference operator*() const {
            return *iter;
        }

        pointer operator->() const {
            return &operator*();
        }

        Self& operator++() {
            --iter;
            return *this;
        }

        Self operator++ (int i) {
            auto tmp = *this;
            --iter;
            return tmp;
        }

        Self& operator--() {
            ++iter;
            return *this;
        }

        Self operator-- (int i) {
            auto tmp = *this;
            --iter;
            return tmp;
        }

        Iter real() const noexcept {
            return iter;
        }

    private:
        Iter iter{};
    };


    template <typename Iter>
    auto reverse_iters_range (Iter beg, Iter end) {
        --beg;
        --end;

        ReverseIterator<Iter> rbeg (end);
        ReverseIterator<Iter> rend (beg);

        return algs::make_pair (rbeg, rend);
    }
}

#endif