#ifndef INSERTER_ITERATOR_TEMPLATE_H_GUARD
#define INSERTER_ITERATOR_TEMPLATE_H_GUARD

#include "iterators_general.h"

namespace data_struct
{
    template <typename T, typename Container>
    class InserterIterator
    {
        using Self = InserterIterator;
        using ConstIter = typename Container::const_iterator;

    public:
        using iterator_category = std::output_iterator_tag;
        using difference_type   = void;

        using value_type = T;
        using reference  = T&;
        using pointer    = T*;
    
    public:
        struct Inserter {
            void operator= (T const& value) {
                it = container.insert (it, value);
            }

            void operator= (T&& value) {
                it = container.insert (it, std::move (value));
            }
            
            Container& container;
            ConstIter& it;
        };

    public:
        explicit InserterIterator (Container& cont, ConstIter it_)
            : container (cont)
            , it (it_)
        {}

        Inserter operator*() {
            if (needInc) {
                throw std::invalid_argument ("aaa");
            }
            needInc = true;
            return Inserter {container, it};
        }

        Self& operator++ () {
            if (not needInc) {
                throw std::invalid_argument ("bbb");
            }
            needInc = false;
            ++it;
            return *this;
        }

        Self operator++ (int i) {
            return Self {container, it}; 
        }
    
    private:
        Container& container;
        ConstIter it{};
        bool needInc = false;
    };
}

#endif