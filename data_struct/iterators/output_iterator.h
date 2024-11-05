#ifndef OUTPUT_ITERATOR_TEMPLATE_H_GUARD
#define OUTPUT_ITERATOR_TEMPLATE_H_GUARD

#include "iterator_traits.h"

namespace iter
{
    template <typename IterImpl>
    class OutputIterator
    {
        using Self = OutputIterator;
        using Value = typename IterImpl::Value;
        using OutType = typename IterImpl::OutType;

        template <typename... Args>
        using EnableIfConvertible = std::enable_if_t<std::is_constructible_v<IterImpl, Args...>>;

    public:
        using iterator_category = std::output_iterator_tag;
        using difference_type   = void;

        using value_type = Value;
        using reference  = Value&;
        using pointer    = Value*;

    public:
        template <typename... Args, typename = EnableIfConvertible<Args...>>
        OutputIterator (Args&&... args) 
            : impl (std::forward<Args> (args)...)
        {}

        OutType operator*() {
            if (not outputIsAvail) {
                throw std::invalid_argument ("aaa");
            }

            outputIsAvail = false;
            return *impl;
        }

        Self& operator++ () {
            if (outputIsAvail) {
                throw std::invalid_argument ("bbb");
            }

            outputIsAvail = true;
            return *this;
        }

        Self operator++ (int i) {
            return *this; 
        }
    
    private:
        IterImpl impl;
        bool outputIsAvail = true;
    };
}
#endif