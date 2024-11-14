#ifndef ITERATOR_BY_PAGES_H_GUARD
#define ITERATOR_BY_PAGES_H_GUARD

#include <string>
#include <cstdint>
#include "table_fwd.h"
#include "../data_struct/list.h"


class IteratorByRows {
    using Self = IteratorByRows;
    using Rows = ds::List<std::string>;
    using ListIter = Rows::const_iterator;

public:
    Self& operator= (Self const&) = delete;
    IteratorByRows (Self const&) = delete;

    IteratorByRows() noexcept = default;
    explicit IteratorByRows (Table const&, TMode);
    IteratorByRows (Self&&) noexcept;
    ~IteratorByRows() noexcept;

    Self& operator= (Self&&) noexcept;

    friend bool operator== (Self const&, Self const&) noexcept;
    friend bool operator!= (Self const&, Self const&) noexcept;

    Self& operator++();

    bool is_end() const noexcept;
    void reset() noexcept;
    void restart() noexcept;

    StringView operator[] (std::string const&) const;
    std::string const& operator*() const noexcept;

    void erase() const;

private:
    struct IterImpl;
    IterImpl* pimpl = nullptr;
};


#endif