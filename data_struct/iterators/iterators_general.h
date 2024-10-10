#ifndef MY_ITERATORS_FWD_H_GUARD
#define MY_ITERATORS_FWD_H_GUARD

#include <type_traits>
#include <iterator>
#include <utility>
// #include "../my_algorithm.h"

namespace data_struct
{
    struct Mutable_tag;
    struct Const_tag;

    template <typename...>
    using Void = void;


    template <typename Iter, typename = Void<>>
    struct IterTraits {};


    template <typename Iter>
    struct IterTraits<Iter, Void<
      typename Iter::iterator_category
    , typename Iter::difference_type
    , typename Iter::value_type
    , typename Iter::reference
    , typename Iter::pointer
    >>
    {
        using Category   = typename Iter::iterator_category;
        using Difference = typename Iter::difference_type;

        using Value     = typename Iter::value_type;
        using Reference = typename Iter::reference;
        using Pointer   = typename Iter::pointer;
    };


    // template <typename Iter>
    // struct IterTraits : public IterTraits_impl<Iter>
    // {};


    template <typename T>
    struct IterTraits<T*> {
        using Category   = std::random_access_iterator_tag;
        using Difference = std::ptrdiff_t;

        using Value     = std::remove_cv_t<T>;
        using Reference = T&;
        using Pointer   = T*; 
    };      


    template <typename Iter>
    using EnableIfRandom = std::enable_if_t<
        std::is_base_of<
            std::random_access_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;


    template <typename Iter>
    using EnableIfBidirectional = std::enable_if_t<
        std::is_base_of<
            std::bidirectional_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;


    template <typename Iter>
    using EnableIfForward = std::enable_if_t<
        std::is_base_of<
            std::forward_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;


    template <typename Iter>
    using EnableIfInput = std::enable_if_t<
        std::is_base_of<
            std::input_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;


    template <typename Iter>
    using EnableIfOutput = std::enable_if_t<
        std::is_base_of<
            std::output_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value

        or

        std::is_base_of<
            std::input_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;
}

#endif