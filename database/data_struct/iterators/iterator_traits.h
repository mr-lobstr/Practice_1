#ifndef MY_ITERATORS_FWD_H_GUARD
#define MY_ITERATORS_FWD_H_GUARD

#include <type_traits>
#include <iterator>
#include <utility>

namespace iter
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


    template <typename T>
    struct IterTraits<T*, Void<>> {
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
    using EnableIfBidirect = std::enable_if_t<
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
            std::forward_iterator_tag
          , typename IterTraits<Iter>::Category
        >::value
    >;


    template <
        template<typename, typename> typename
      , typename...
    >
    struct ForTwoAndMore_ {
        using type = std::false_type;
    };


    template <
        template<typename, typename> typename Traits
      , typename T1
      , typename T2
    >
    struct ForTwoAndMore_<Traits, T1, T2> {
        using type = Traits<T1, T2>;
    };


    template <
        template<typename, typename> typename Traits
      , typename T
      , typename... Args
    >
    struct ForTwoAndMore
        : ForTwoAndMore_<Traits, T, Args...>
    {};


    template <
        template<typename, typename> typename Traits
      , typename T
      , typename... Args
    >
    using ForTwoAndMore_t = typename ForTwoAndMore<Traits, T, Args...>::type;


    template <
        template<typename, typename> typename Traits
      , typename T
      , typename... Args
    >
    constexpr bool ForTwoAndMore_v = ForTwoAndMore_t<Traits, T, Args...>::value;


    template <typename T1, typename T2>
    struct IsBaseOf
        : std::is_base_of<T1, std::remove_reference_t<T2>>
    {};


    template <typename T1, typename T2>
    struct IsSame
        : std::is_same<T1, std::remove_reference_t<T2>>
    {};


    template <typename T, typename... Args>
    constexpr bool IsConstructibleButNotBase_v = 
                                    ForTwoAndMore_v<IsSame, T, Args...>
                                or 
                                    not ForTwoAndMore_v<IsBaseOf, T, Args...>
                                and
                                    std::is_constructible_v<T, Args...>;
}

#endif