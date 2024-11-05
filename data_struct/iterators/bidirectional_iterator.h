#ifndef BIDIRECTIONAL_ITERATOR_TEMPLATE_H_GUARD
#define BIDIRECTIONAL_ITERATOR_TEMPLATE_H_GUARD

#include "forward_iterator.h"

namespace iter
{
    template <typename T, typename Impl, typename Mut>
    class BidirectIterator
        : public ForwardIterator<T, Impl, Mut>
    {
        using Base = ForwardIterator<T, Impl, Mut>;
        using Self = BidirectIterator;

        template <typename... Args>
        using EnableIfImpl = std::enable_if_t<
            IsConstructibleButNotBase_v <Impl, Args...>
        >;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        
        using typename Base::difference_type;
        using typename Base::value_type;
        using typename Base::reference;
        using typename Base::pointer;

    public:
        using Base::operator*;
        using Base::operator->;
        using Base::operator++;

    public:
        BidirectIterator() = default;

        template <typename... Args, typename = EnableIfImpl<Args...>>
        BidirectIterator (Args&&... args)
            : Base (std::forward<Args> (args)...)
        {}

        template <typename Mut_>
        BidirectIterator (BidirectIterator<T, Impl, Mut_> const& it)  
            : Base ((Base const&) it)
        {}

        Self& operator--() {
            Impl::prev();
            return *this;
        }

        Self operator-- (int i) {
            auto tmp = *this;
            Impl::prev();
            return tmp;
        }
    };
}

#endif