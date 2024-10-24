#ifndef REVERSE_ITERATOR_TEMPLATE_H_GUARD
#define REVERSE_ITERATOR_TEMPLATE_H_GUARD

#include "iterators_general.h"

namespace data_struct
{
    template <typename Iter>
    class ReverseIterator
    {
        using Self = ReverseIterator;

    public:
        using iterator_category = typename Iter::iterator_category;
        using difference_type   = typename Iter::difference_type;

        using value_type = typename Iter::value_type;
        using reference  = typename Iter::reference;
        using pointer    = typename Iter::pointer;

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

}

#endif