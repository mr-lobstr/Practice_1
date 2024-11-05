#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "file_manager.h"
#include "iterator_by_rows.h"
#include "../data_struct/hash_table.h"

class Database;

class Table {
    friend class TableStateGuard;

    using Columns = data_struct::HashTable<std::string, std::size_t>;

    friend TableFileManager;
    friend IteratorByRows;

public:
    using Iterator = IteratorByRows;

    enum Lock {
        lock = true
      , unlock = false
    };

public:
    Table (std::string const&, Database const&);
    ~Table() noexcept = default;
    
    template <typename Iter>
    Table& set_columns (Iter beg, Iter end) {
        size_t index = 1;

        algs::for_each (beg, end, [&] (auto& column) {
            columns.add (column, index++);
        });
        
        return *this;
    }

    void create_files() const;

    Iterator begin() const;
    Iterator end() const;

    void insert_back (std::string const&);
    void erase (Iterator&);

private:
    std::size_t rows_limit() const noexcept;
    std::string path_dir() const;
    std::string header() const;

private:
    std::string name{};
    Columns columns{};
    TableFileManager fm;

    Database const& database;
};


#endif