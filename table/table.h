#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "table_fwd.h"
#include "file_manager.h"
#include "iterator_by_rows.h"
#include "table_state.h"
#include "../data_struct/hash_table.h"

struct TConfig {
    TableName name;
    ds::DynamicArray<Column> columns;
};

class Table {
    friend TFileManager;
    friend IteratorByRows;
    friend TState;

public:
    using Iterator = IteratorByRows;
    using Row = ds::DynamicArray<StringView>;

public:
    Table (Database const&, TConfig const&);

    bool has_column (std::string const&) const noexcept;

    std::size_t rows_limit() const noexcept;
    std::string path_dir() const;
    std::string header() const;

    Iterator make_iter (TMode) const;

    StringView get_element_from (Row, Column) const;

    void insert_back (Row const&);
    void erase (Iterator&);

private:
    Database const& database;

    std::string name{};
    ds::HashTable<Column, ColumnNumb> columns{};

    TFileManager fm;
    TState state;
};

#endif