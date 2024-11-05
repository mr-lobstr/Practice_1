#include <string>
#include <fstream>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "database.h"

#include "table/iterator_by_rows.cpp"
#include "table/file_manager.cpp"
#include "table/table.cpp"
#include "data_struct/string_view.cpp"
#include "table/table_state_guard.cpp"

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
        table.create_files ();
    }
}


int main()
{
    Database ("./", "schema.json");
}