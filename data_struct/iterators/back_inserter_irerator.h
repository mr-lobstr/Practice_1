#ifndef BACK_INSERTER_ITERATOR_TEMPLATE_H_GUARD
#define BACK_INSERTER_ITERATOR_TEMPLATE_H_GUARD

#include "iterators_general.h"

namespace data_struct
{
    template <typename T, typename Container>
    class BackInserterIterator {
        using Self = BackInserterIterator;

    public:
        using iterator_category = std::output_iterator_tag;
        using difference_type   = void;

        using value_type = T;
        using reference  = T&;
        using pointer    = T*;
    
    public:
        struct Inserter {
            void operator= (T const& value) {
                container.push_back (value);
            }

            void operator= (T&& value) {
                container.push_back (std::move (value));
            }
            
            Container& container;
        };

    public:
        explicit BackInserterIterator (Container& cont)
            : container (cont)
        {}

        Inserter operator*() {
            if (needInc) {
                throw std::invalid_argument ("aaa");
            }
            needInc = true;
            return Inserter {container};
        }

        Self& operator++ () {
            if (not needInc) {
                throw std::invalid_argument ("bbb");
            }
            needInc = false;
            return *this;
        }

        Self operator++ (int i) {
            return Self {container};
        }
    
    private:
        Container& container;
        bool needInc = false;
    };
}

#endif