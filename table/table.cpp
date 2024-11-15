#include "table.h"
#include "../database/database.h"
#include "../data_struct/string_view.h"

using namespace std;
using namespace data_struct;


auto set_columns (DynamicArray<Column> const& columns) {
    HashTable<Column, ColumnNumb> result;
    size_t index = 1;

    for (auto& column : columns) {
        result.add (column, index++);
    }

    return result;
}


Table::Table (Database const& db, TConfig const& config)
    : database (db)
    , name (config.name)
    , columns (set_columns (config.columns))
    , fm (*this)
    , state (*this)
{}


bool Table::has_column (std::string const& columnName) const noexcept {
    return columns.find (columnName) != columns.end()
        or columnName == name + "_pk";
}


Table::Iterator Table::make_iter (TMode mode) const {
    return Iterator (*this, mode);
}


void Table::insert_back (Row const& row) {
    TStateGuard guard (state, TMode::writing);

    auto expected = columns.size();
    auto recieved = row.size();

    if (expected != recieved) throw invalid_argument (
        "неверное количество аргументов: ожидалось " + to_string (expected) + ", получено " + to_string (recieved) + "\n"
    );

    state.row_writing_start();
    
    fm.add_page (state.current_page(), [&] (auto& fPage) {
        fPage << state.current_pk() << "," << join_views (row, ",") << "\n";
    });

    state.row_writing_finish();
}


StringView Table::get_element_from (Row row, Column column) const {
    if (column == name + "_pk") {
        return row[0];
    } else {
        try {
            auto ind = columns[column];
            return row[ind];
        } catch (invalid_argument const& e) {
            throw runtime_error (
                e.what() + column + "\n"
            );
        }
    }
}


size_t Table::rows_limit() const noexcept {
    return database.rowsLimit;
}
    
    
string Table::path_dir() const {
    return database.pathDir + name;
}