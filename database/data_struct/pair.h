#ifndef MY_PAIR_H_GUARD
#define MY_PAIR_H_GUARD

namespace data_struct
{
    template <typename T, typename U>
    struct Pair {
        T first;
        U second;
    };
}


namespace algs
{
    template <typename T, typename U>
    auto make_pair (T const& t, U const& u) {
        return data_struct::Pair<T, U> {t, u};
    }
}

#endif