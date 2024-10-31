#ifndef ITERATOR_BY_PAGES_H_GUARD
#define ITERATOR_BY_PAGES_H_GUARD

#include <string>
#include <cstdint>
#include "data_struct/string_view.h"
#include "data_struct/list.h"

class IteratorByRows {
    using Self = IteratorByRows;
    using Rows = data_struct::List<std::string>;
    using ListIter = typename Rows::const_iterator;

    friend class Table;

public:
    IteratorByRows() noexcept = default;
    explicit IteratorByRows (Table const&);
    IteratorByRows (Self&&);
    Self& operator= (Self&&);
    ~IteratorByRows();   

    Self& operator= (Self const&) = delete;
    IteratorByRows (Self const&) = delete;

    friend bool operator== (Self const&, Self const&) noexcept;
    friend bool operator!= (Self const&, Self const&) noexcept;

    bool is_end() const noexcept;
    void reset() noexcept;

    StringView operator[] (std::string const&) const;
    std::string const& operator*() const noexcept;

    Self& operator++();

private:
    void erase();

private:
    struct IterImpl;

    IterImpl* pimpl = nullptr;
};


#endif