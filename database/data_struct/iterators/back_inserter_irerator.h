#ifndef BACK_INSERTER_ITERATOR_TEMPLATE_H_GUARD
#define BACK_INSERTER_ITERATOR_TEMPLATE_H_GUARD

#include "output_iterator.h"

namespace iter
{
    template <typename Container>
    struct BackInserterImpl {
        using OutType = BackInserterImpl&;
        using Value = typename Container::value_type;

    public:
        BackInserterImpl (Container& container_) noexcept
            : container (container_)
        {}

        void operator= (Value const& value) {
            container.push_back (value);
        }

        void operator= (Value&& value) {
            container.push_back (std::move (value));
        }

        OutType operator*() noexcept {
            return *this;
        }

    private:
        Container& container;
    };


    template <typename Container>
    auto back_inserter (Container& container) {
        using Impl = BackInserterImpl<Container>;
        return OutputIterator<Impl> (container);
    }
}

#endif