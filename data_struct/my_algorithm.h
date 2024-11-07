#ifndef MY_ALGORITHM_H_GUARD
#define MY_ALGORITHM_H_GUARD

#include "iterators.h"

namespace algs
{
    template <typename Iter, typename Action>
    void for_each (Iter beg, Iter end, Action action) {
        while (beg != end) {
            action (*beg);
            ++beg;
        }
    }


    template <typename InputIter, typename OutputIter, typename Action>
    void transform (InputIter beg, InputIter end, OutputIter out, Action action) {
        while (beg != end) {
            *out = action (*beg);
            ++beg;
            ++out;
        }
    }


    template <typename T>
    void init (T* beg, T* end, T const& value) {
        while (beg != end) {
            new (beg) T {value};
            ++beg;
        }
    }


    template <typename T>
    void copy_init (T* beg, T* end, T* out) {
        while (beg != end) {
            new (out) T {*beg};
            ++beg;
            ++out;
        }
    }


    template <typename InputIter, typename T>
    void range_init_move (InputIter beg, InputIter end, T* out) {
        while (beg != end) {
            new (out) T {std::move (*beg)};
            ++beg;
            ++out;
        }
    }


    template <typename InputIter, typename OutputIter>
    void copy (InputIter beg, InputIter end, OutputIter out) {
        while (beg != end) {
            *out = *beg++;
            ++out;
        }
    }


    template <typename InputIter, typename OutputIter>
    void move (InputIter beg, InputIter end, OutputIter out) {
        while (beg != end) {
            *out++ = std::move (*beg++);
        }
    }


    template <typename Iter>
    void shift_left (Iter beg, Iter end) {
        auto oldBeg = beg++;
        algs::move (beg, end, oldBeg);
    }


    template <typename Iter>
    void shift_right (Iter beg, Iter end) {
        auto [rbeg, rend] = iter::reverse_iters_range (beg, end);

        shift_left (rbeg, rend);
    }


    template <typename Iter, typename T>
    Iter find (Iter beg, Iter end, T const& value) {
        while (beg != end) {
            if (*beg == value)
                return beg;
            ++beg;
        }
        return end;
    }


    template <typename Iter, typename Predicate>
    Iter find_if (Iter beg, Iter end, Predicate pred) {
        while (beg != end) {
            if (pred (*beg))
                return beg;
            ++beg;
        }
        return end;
    }


    template <typename Iter, typename T>
    auto count (Iter beg, Iter end, T const& value) {
        std::size_t cnt = 0;

        while (beg != end) {
            if (*beg == value) {
                ++cnt;
            }
            ++beg;
        }

        return cnt;
    }
}

#endif