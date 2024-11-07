#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "file_manager.h"
#include "iterator_by_rows.h"
#include "../data_struct/hash_table.h"
namespace ds = data_struct;

class Database;

class Table {
    friend TableFileManager;
    friend IteratorByRows;
    friend class TableStateGuard;

public:
    using Iterator = IteratorByRows;
    using Row = ds::DynamicArray<StringView>;

public:
    Table (std::string const&, Database const&);

    bool has_column (std::string const&) const noexcept;
    std::size_t rows_limit() const noexcept;
    std::string path_dir() const;
    std::string header() const;
    
    template <typename Iter>
    Table& set_columns (Iter, Iter);

    void create_files() const;

    Iterator begin() const;
    Iterator end() const;

    void insert_back (Row const&);
    void erase (Iterator&);

private:
    std::string name{};
    TableFileManager fm;
    ds::HashTable<std::string, std::size_t> columns{};

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