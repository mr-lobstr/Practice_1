#ifndef MY_ITERATORS_H_GUARD
#define MY_ITERATORS_H_GUARD

#include "iterators/iterators_general.h"
#include "iterators/forward_iterator.h"
#include "iterators/bidirectional_iterator.h"
#include "iterators/random_iterator.h"
#include "iterators/back_inserter_irerator.h"
#include "iterators/inserter_iterator.h"

namespace data_struct
{
    template <typename T, typename Impl, typename Mut>
    using BaseContainerIter = ForwardIterator<T, Impl, Mut>;


    template <typename Iter>
    Iter next_iter (Iter it) noexcept {
        return ++it;
    }


    template <typename Container>
    using get_iterator_t = typename Container::iterator;


    template <typename Container>
    using get_const_iterator_t = typename Container::const_iterator;
}

#endif