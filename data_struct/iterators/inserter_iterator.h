#ifndef INSERTER_ITERATOR_TEMPLATE_H_GUARD
#define INSERTER_ITERATOR_TEMPLATE_H_GUARD

#include "output_iterator.h"

namespace iter
{
    template <typename Container>
    struct InserterImpl {
        using OutType = InserterImpl&;
        using Value = typename Container::value_type;
        using CIter = typename Container::const_iterator;

    public:
        InserterImpl (Container& container_, CIter pos) noexcept
            : container (container_)
            , insertPos (pos)
        {}

        void operator= (Value const& value) {
            insertPos = container.insert (insertPos, value);
            ++insertPos;
        }

        void operator= (Value&& value) {
            insertPos = container.insert (insertPos, std::move (value));
            ++insertPos;
        }

        OutType operator*() noexcept {
            return *this;
        }

    private:
        Container& container;
        CIter insertPos{};
    };


    template <typename Container>
    auto inserter (Container& container, typename Container::const_iterator cit) {
        using Impl = InserterImpl<Container>;
        return OutputIterator<Impl> (container, cit);
    }
}

#endif