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

    void select (TablesNames const&, TableColumnPairs const&);
    void insert (std::string const&, Table::Row const&);
    void erase (std::string const&, Conditions);

private:
    Table& get_table (std::string const&);

    void tables_columns_check (TableColumnPairs const&);
    std::string select_table_name (TablesNames const&);
    void create_select (std::string const&, TableColumnPairs const&);

private:
    std::string pathDir{};
    std::size_t rowsLimit;
    std::size_t maxCntPagesLoaded = 100;

    ds::HashTable<std::string, Table> tables{};
};


#endif