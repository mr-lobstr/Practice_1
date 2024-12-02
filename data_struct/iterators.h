#ifndef MY_ITERATORS_H_GUARD
#define MY_ITERATORS_H_GUARD

#include "iterators/iterator_traits.h"
#include "iterators/forward_iterator.h"
#include "iterators/bidirectional_iterator.h"
#include "iterators/random_iterator.h"
#include "iterators/back_inserter_irerator.h"
#include "iterators/inserter_iterator.h"
#include "iterators/add_iterator.h"
#include "iterators/reverse_iterator.h"

namespace iter
{
    template <typename T, typename Impl, typename Mut>
    using BaseContainerIter = ForwardIterator<T, Impl, Mut>;


    template <typename Iter>
    Iter next_iter (Iter it) noexcept {
        return ++it;
    }


    template <typename Iter>
    Iter advance (Iter it, size_t n) noexcept {
        while (n--) {
            ++it;
        }

        return it;
    }
}

#endif