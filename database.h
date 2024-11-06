#ifndef DATABASE_H_GUARD
#define DATABASE_H_GUARD

#include <string>
#include "conditions_parser.h"
#include "table/table.h"
#include "data_struct/hash_table.h"
#include "iterator_with_condition.h"

namespace ds = data_struct;

using TableColumnPairs = ds::DynamicArray<ds::Pair<StringView, StringView>>;

class Database {
    friend Table;
    friend class CartesianIterator;

public:
    Database (std::string const&, std::string const&);

    void insert (std::string const&, Table::Row const&);
    void erase (std::string const&, Conditions);
    void select (TablesNames const&, TableColumnPairs const&);
    void filter (TablesNames const&, TableColumnPairs const&, Conditions&);

    bool has_table (std::string const&) const noexcept;

private:
    void excpetion_if_hasnot_table (std::string const&) const;

    void tables_columns_check (TableColumnPairs const&);
    std::string select_file_name (TablesNames const&);
    void create_select (std::string const&, TableColumnPairs const&);

private:
    std::string pathDir{};
    std::size_t rowsLimit;
    std::size_t maxCntPagesLoaded = 100;

    ds::HashTable<std::string, Table> tables{};
};


#endif