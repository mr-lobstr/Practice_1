#ifndef CARTESIAN_ITERATOR_H_GUARD
#define CARTESIAN_ITERATOR_H_GUARD

#include <string>
#include "data_struct/dynamic_array.h"
#include "table/table.h"
namespace ds = data_struct;

class Database;

using TablesNames = ds::DynamicArray<std::string>;

class CartesianIterator {
public:
    CartesianIterator (Database&, TablesNames const&);
    ~CartesianIterator() noexcept;

    Table::Iterator const& operator[] (std::string const&) const;

    void operator++();
    bool is_end() const noexcept;

    void reset() noexcept;

private:
    Table::Iterator get_table_begin (std::string const&) const;

private:
    bool isEnd = false;
    Database& database;
    ds::DynamicArray<ds::Pair<std::string, Table::Iterator>> tablesIters;
};



#endif