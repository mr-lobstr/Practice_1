#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "table_fwd.h"
#include "file_manager.h"
#include "iterator_by_rows.h"
#include "table_state.h"
#include "../data_struct/hash_table.h"


class Table {
    friend TFileManager;
    friend IteratorByRows;
    friend TState;

public:
    using Iterator = IteratorByRows;
    using Row = ds::DynamicArray<StringView>;

public:
    Table (std::string const&, Database const&);
    
    void init() const;

    bool has_column (std::string const&) const noexcept;

    std::size_t rows_limit() const noexcept;
    std::string path_dir() const;
    std::string header() const;
    
    template <typename Iter>
    Table& set_columns (Iter, Iter);

    Iterator make_iter (TMode) const;

    StringView get_element_from (Row, Column) const;

    void insert_back (Row const&);
    void erase (Iterator&);

private:
    TFileManager fm;
    TState state;

    std::string name{};
    ds::HashTable<Column, ColumnNumb> columns{};

    Database const& database;
};


template <typename Iter>
Table& Table::set_columns (Iter beg, Iter end) {
    size_t index = 1;

    algs::for_each (beg, end, [&] (auto& column) {
        columns.add (column, index++);
    });
        
    return *this;
}

#endif