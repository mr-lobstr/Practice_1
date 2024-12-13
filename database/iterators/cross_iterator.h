#ifndef CARTESIAN_ITERATOR_H_GUARD
#define CARTESIAN_ITERATOR_H_GUARD

#include <string>
#include "../database/database_fwd.h"
#include "../table/table.h"
#include "../data_struct/dynamic_array.h"


class CrossIterator {
    struct TCI {
        TableName table;
        Table::Iterator iter;
    };

public:
    CrossIterator (Database&, TablesNames const&, TableColumnPairs const&);
    ~CrossIterator() noexcept;

    Table::Iterator const& operator[] (Column const&) const;
    std::string cross() const;

    void operator++();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    Database& database;
    TableColumnPairs tcPairs;
    ds::DynamicArray<TCI> tablesIters;

    bool isEnd = false;
};

#endif