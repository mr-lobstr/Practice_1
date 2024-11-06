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

    void check_column (std::string const&);

    void create_files() const;

    Iterator begin() const;
    Iterator end() const;

    using Row = ds::DynamicArray<StringView>;

    void insert_back (Row const&);
    void erase (Iterator&);

private:
    std::size_t rows_limit() const noexcept;
    std::string path_dir() const;
    std::string header() const;

private:
    std::string name{};
    TableFileManager fm;
    ds::HashTable<std::string, std::size_t> columns{};

    Database const& database;
};


#endif