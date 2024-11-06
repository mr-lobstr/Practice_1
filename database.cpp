#include <string>
#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "database.h"
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
        table.create_files();
    }
}


void Database::insert (std::string const& tableName, Table::Row const& row) {
    try {
        tables[tableName].insert_back (row);
    } catch (invalid_argument const& e) {
        cerr << "ошибка при выполнении INSERT:\n"
             << tableName << ": " << e.what() << endl;
    } catch (runtime_error const& re) {
        cerr << "ошибка при выполнении INSERT:\n"
             << re.what() << endl;
    }
}


void Database::erase (std::string const& tableName, Conditions condition) {
    try {
        tables[tableName];
    } catch (invalid_argument const& e) {
        cerr << "ошибка при выполнении DELETE:\n"
             << tableName << ": " << e.what() << endl;
    }

    try {
        auto it = IteratorWithCondition (*this, {tableName}, condition);

        while (not it.is_end()) {
            it[tableName].erase();
        }
    } catch (runtime_error const& re) {
        cerr << "ошибка при выполнении DELETE:\n"
             << tableName << ": " << re.what() << endl;
    }
}


void Database::select (TablesNames const& tNames, TableColumnPairs const& tcPairs) {
    tables_columns_check (tcPairs);
    auto selectName = select_table_name (tNames);
    
    ofstream select (selectName);
    bool isFirst = true;

    for (auto& [_, columnName] : tcPairs) {
        select << (isFirst ? "" : ", ")
               << columnName;
        isFirst = false;
    }
    select << endl;
    isFirst = true;

    auto it = CartesianIterator (*this, tNames);

    while (not it.is_end()) {
        for (auto& [tableName, columnName] : tcPairs) {
            select << (isFirst ? "" : ", ")
                   << it[tableName][columnName];
            isFirst = false;
        }
        select << endl;
        ++it;
    }

    select.close();
}


Table& Database::get_table (string const& tableName) {
    try {
        return tables [tableName];
    } catch (invalid_argument const& e) {
        cerr << tableName << ": ";
    }
}


void Database::tables_columns_check (TableColumnPairs const& tcPairs) {
    for (auto& [tableName, columnName] : tcPairs) {
        get_table(tableName).check_column (columnName);
    }
}


std::string Database::select_table_name (TablesNames const& tNames) {
    string name = "select";

    for (auto& tName : tNames) {
        name += "_" + tName;
    }

    name.pop_back();
    return name;
}


void Database::create_select (string const& tableName, TableColumnPairs const& tcPairs) {
    auto& table = tables.emplace_add (tableName, tableName, *this)->value;
    table.set_columns (columns.begin(), columns.end());
    table.create_files();
}