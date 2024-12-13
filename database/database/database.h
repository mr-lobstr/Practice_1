#ifndef DATABASE_H_GUARD
#define DATABASE_H_GUARD

#include <string>
#include "database_fwd.h"
#include "../request/request_base.h"
#include "../table/table.h"
#include "../data_struct/hash_table.h"


class Database {
    friend Table;
    friend class CrossIterator;
    friend class DeleteIterator;

public:
    Database (std::string const&, std::string const&);

    bool has_table (std::string const&) const noexcept;

    std::string execute_request (std::string const&);

private:
    void create_table (TableName const&, Columns const&);

    std::string execute (Request const&);

    std::string add_table (Request const&);
    std::string insert (Request const&);
    std::string erase (Request const&);
    std::string select (Request const&);

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