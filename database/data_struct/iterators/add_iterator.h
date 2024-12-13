#ifndef ADD_ITERATOR_TEMPLATE_H_GUARD
#define ADD_ITERATOR_TEMPLATE_H_GUARD

#include "output_iterator.h"

namespace iter
{
    template <typename Container>
    struct AddIteratorImpl {
        using OutType = AddIteratorImpl&;
        using Value = typename Container::value_type;

    public:
        AddIteratorImpl (Container& container_) noexcept
            : container (container_)
        {}

        void operator= (Value const& value) {
            container.add (value);
        }

        void operator= (Value&& value) {
            container.add (std::move (value));
        }

        OutType operator*() noexcept {
            return *this;
        }

    private:
        Container& container;
    };


    template <typename Container>
    auto add_iterator (Container& container) {
        using Impl = AddIteratorImpl<Container>;
        return OutputIterator<Impl> (container);
    }
}

#endif