#include <filesystem>
#include <nlohmann/json.hpp>

#include "database.h"

#include "../parser/parser.h"
#include "../request/request.h"
#include "../iterators/delete_iterator.h"
#include "../iterators/filter_iterator.h"
#include "../table/table.h"
#include "../utility/utility.h"

using namespace std;
using namespace std::filesystem;
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

    create_directory (resp["name"]);

    for (auto it = jTables.begin(); it != jTables.end(); ++it) {
        auto const& tName = it.key();
        auto const& columns = it.value();

        create_table (tName, {columns.begin(), columns.end()});
    }
}


void Database::create_table (TableName const& tName, Columns const& columns) {
    tables.emplace_add (tName, *this, TConfig {
        tName
      , columns
    });
}


string Database::execute_request (string const& strRequest) {
    RequestPtr requestPtr;
    Parser parser;

    try {
        requestPtr = parser.parse (strRequest);
    } catch (exception const& e) {
        return "Err\nво время разбора запроса возникла ошибка:\n"s + e.what();
    }

    try {
        return execute (*requestPtr);
    } catch (exception const& e) {
        return "Err\n"s + e.what();
    }
}


string Database::execute (Request const& request) {
    switch (request.type)
    {
        case Request::Type::AddTable:
            return add_table (request);

        case Request::Type::Insert:
            return insert (request);

        case Request::Type::Delete:
            return erase (request);

        case Request::Type::Select:
            return select (request);
    
        default:
            break;
    }
}


string Database::add_table (Request const& request_) {
    auto& request = dynamic_cast<AddRequest const&> (request_);

    try {
        create_table (request.tableName, request.columns);
    } catch (exception const& e) {
        throw runtime_error (
            "ошибка при создании таблицы " + request.tableName + ":\n"s + e.what() + "\n"
        );
    }

    return "Ok\n";
}

#include <iostream>

string Database::insert (Request const& request_) {
    auto& request = dynamic_cast<InsertRequest const&> (request_);
    PrimeKey pk;

    try {
        excpetion_if_hasnot_table (request.tableName);
        pk = tables[request.tableName].insert_back (request.row);
    } catch (exception const& e) {
        throw runtime_error (
            "ошибка при выполнении INSERT:\n"s + e.what() + "\n"
        );
    }

    return "Ok\n" + to_string (pk);
}


string Database::erase (Request const& request_) {
    auto& request = dynamic_cast<DeleteRequest const&> (request_);

    try {
        excpetion_if_hasnot_table (request.tableName);

        auto it = DeleteIterator (*this, request.tableName, request.condition);

        while (not it.is_end()) {
            ++it;
        }
    } catch (exception const& e) {
        throw runtime_error (
            "ошибка при выполнении DELETE:\n"s + e.what() + "\n"
        );
    }

    return "Ok\n";
}


string Database::select (Request const& request_) {
    auto& request = dynamic_cast<SelectRequest const&> (request_);
    string result = "Ok\n";

    try {
        tables_columns_check (request.tcPairs);
        auto it = FilterIterator (*this, request.tablesNames, request.tcPairs, request.condition);

        bool intoString = request.outTable.empty();

        for (; not it.is_end(); ++it) {
            if (intoString) {
                result += it.cross() + "\n";
            } else {
                InsertRequest req;
                req.tableName = request.outTable;
                req.row = sv_split (it.cross(), ',');
                insert (req);
            }
        }
    } catch (exception const& e) {
        throw runtime_error (
            "ошибка при выполнении SELECT:\n"s + e.what() + "\n"
        );
    }

    return result;
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
                "таблица \'" + tableName + "\' не содержит колонку \'" + columnName + "\'\n"
            );
        }
    }
}