#ifndef BIDIRECTIONAL_ITERATOR_TEMPLATE_H_GUARD
#define BIDIRECTIONAL_ITERATOR_TEMPLATE_H_GUARD

#include "forward_iterator.h"

namespace data_struct
{
    template <typename T, typename Impl, typename Mut>
    class BidirectionalIterator
        : public ForwardIterator<T, Impl, Mut>
    {
        using Base = ForwardIterator<T, Impl, Mut>;
        using Self = BidirectionalIterator;

        friend typename Impl::Container;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        
        using typename Base::difference_type;
        using typename Base::value_type;
        using typename Base::reference;
        using typename Base::pointer;

    public:
        using Base::impl;

        using Base::operator*;
        using Base::operator->;
        using Base::operator++;
        using Base::real;

    public:
        BidirectionalIterator() = default;

        BidirectionalIterator (Impl impl_) 
            : Base (impl_)
        {}

        template <typename Mut_>
        BidirectionalIterator (BidirectionalIterator<T, Impl, Mut_> const& it)  
            : Base (it)
        {}

        Self& operator--() {
            impl.prev();
            return *this;
        }

        Self operator-- (int i) {
            auto tmp = *this;
            impl.prev();
            return tmp;
        }
    };
}

#endif