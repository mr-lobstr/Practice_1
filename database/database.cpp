#include <string>
#include <fstream>
#include <filesystem>
#include "database.h"
#include "../nlohmann/json.hpp"
using namespace std;
using namespace data_struct;
using namespace nlohmann;


Database::Database (string const& path_, string const& schemaName)
{
    fstream jFile (schemaName);
    auto resp = json::parse (jFile);
    jFile.close();

    pathDir = path_ + string (resp["name"]) + "/";
    rowsLimit = resp["tuples_limit"];
    auto const& jTables = resp["structure"];

    filesystem::create_directory (resp["name"]);

    for (auto it = jTables.begin(); it != jTables.end(); ++it) {
        auto const& tableName = it.key();
        auto const& columns = it.value();

        auto& table = tables.emplace_add (tableName, tableName, *this)->value;
        table.set_columns (columns.begin(), columns.end());
        table.init();
    }
}


void Database::insert (std::string const& tableName, Table::Row const& row) {
    try {
        excpetion_if_hasnot_table (tableName);
        tables[tableName].insert_back (row);
    } catch (exception const& re) {
        cerr << "ошибка при выполнении INSERT:\n" << re.what() << endl;
    }
}


void Database::erase (std::string const& tableName, Condition& condition) {
    try {
        excpetion_if_hasnot_table (tableName);

        auto it = IteratorWithCondition (*this, {tableName}, condition, TMode::writing);

        while (not it.is_end()) {
            it[tableName].erase();
            it.validate();
        }
    } catch (exception const& re) {
        cerr << "ошибка при выполнении DELETE:\n" << re.what() << endl;
    }
}


std::string file_name (string name, TablesNames const& tNames) {
    for (auto& tName : tNames) {
        name += "_" + tName;
    }
    
    name +=".txt";
    return name;
}


void print_file_head (ofstream& select, TableColumnPairs const& tcPairs) {
    bool isFirst = true;

    for (auto& [_, columnName] : tcPairs) {
        select << (isFirst ? "" : ", ") << columnName;
        isFirst = false;
    }

    select << endl;
}


template <typename Iter>
void print_file_rows (ofstream& select, Iter& it, TableColumnPairs const& tcPairs) {
    while (not it.is_end()) {
        bool isFirst = true;

        for (auto& [tableName, columnName] : tcPairs) {
            select << (isFirst ? "" : ", ") << it[tableName][columnName];
            isFirst = false;
        }
        
        select << endl;
        ++it;
    }
}


void Database::select (TablesNames const& tNames, TableColumnPairs const& tcPairs) {
    try {
        tables_columns_check (tcPairs);
        ofstream select (file_name ("select", tNames));

        auto it = CartesianIterator (*this, tNames, TMode::reading);
        print_file_head (select, tcPairs);
        print_file_rows (select, it, tcPairs);

        select.close();
    } catch (exception const& re) {
        cerr << "ошибка при выполнении SELECT:\n" << re.what() << endl;
    }
}


void Database::filter (TablesNames const& tNames, TableColumnPairs const& tcPairs, Condition& condition) {
    try {
        tables_columns_check (tcPairs);
        ofstream filter (file_name ("filter", tNames));

        auto it = IteratorWithCondition (*this, tNames, condition, TMode::reading);
        print_file_head (filter, tcPairs);
        print_file_rows (filter, it, tcPairs);

        filter.close();
    } catch (exception const& re) {
        cerr << "ошибка при выполнении FILTER:\n" << re.what() << endl;
    }
}


bool Database::has_table (string const& tableName) const noexcept {
    return tables.find (tableName) != tables.end();
}


void Database::excpetion_if_hasnot_table (std::string const& tableName) const {
    if (not has_table (tableName)) throw invalid_argument (
        "база данных не содержит таблицу \'" + tableName + "\'\n"
    );    
}


void Database::tables_columns_check (TableColumnPairs const& tcPairs) {
    for (auto& [tableName, columnName] : tcPairs) {
        excpetion_if_hasnot_table (tableName);

        if (not tables[tableName].has_column (columnName)) {
            throw invalid_argument (
                "таблица \'" + tableName + "\' не содержит колонку \'" + tableName + "\'\n"
            );
        }
    }
}