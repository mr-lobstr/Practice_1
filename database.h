#ifndef DATABASE_H_GUARD
#define DATABASE_H_GUARD

#include <string>
#include "table/table.h"
#include "data_struct/hash_table.h"

class Database {
    friend Table;

public:
    Database (std::string const&, std::string const&);

private:
    std::string pathDir{};
    std::size_t rowsLimit;
    std::size_t maxCntPagesLoaded = 100;

    data_struct::HashTable<std::string, Table> tables{};
};

#endif