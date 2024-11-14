#ifndef CARTESIAN_ITERATOR_H_GUARD
#define CARTESIAN_ITERATOR_H_GUARD

#include <string>
#include "database_fwd.h"
#include "../table/table.h"
#include "../data_struct/dynamic_array.h"


class CartesianIterator {
    using TableAndIter = ds::Pair<TableName, Table::Iterator>;

public:
    CartesianIterator (Database&, TablesNames const&, TMode);
    ~CartesianIterator() noexcept;

    Table::Iterator const& operator[] (Column const&) const;

    void operator++();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    Database& database;
    ds::DynamicArray<TableAndIter> tablesIters;

    bool isEnd = false;
};

#endif