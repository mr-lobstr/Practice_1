#include "table.h"
#include "../database.h"
#include "table_state_guard.h"
#include "../data_struct/string_view.h"

using namespace std;
using namespace data_struct;

Table::Table (string const& name_, Database const& db)
    : name (name_)
    , fm (*this)
    , database (db)
{}


void Table::create_files() const {
    fm.create_files();
}


Table::Iterator Table::begin() const {
    return Iterator (*this);
}


Table::Iterator Table::end() const {
    return Iterator{};
}


void Table::insert_back (string const& str) {
    TableStateGuard stateGuard (*this);
    auto words = split_into_words (str);

    if (columns.size() != words.size()) {
        auto expected = to_string (columns.size());
        auto recieved = to_string (words.size());

        throw std::runtime_error (
            "ожидалось " + expected + " аргументов, получено " + recieved + "\n"
        );
    }
        
    stateGuard.recording_start();
    
    fm.add_page (stateGuard.current_page(), [&] (auto& fPage) {
        fPage << stateGuard.current_pk() << ","
              << join_views (words) << "\n";
    });

    stateGuard.recording_finish();
}


string Table::header() const {
    DynamicArray<string const*> ptrsColumns (columns.size());

    for (auto& [column, index] : columns) {
        ptrsColumns[index-1] = &column;
    }

    string header = name + "_pk";

    for (auto& pColumn : ptrsColumns) {
        header += ("," + *pColumn);
    }

    return header;
}


void Table::erase (Iterator& it) {
    it.erase();
} 


size_t Table::rows_limit() const noexcept {
    return database.rowsLimit;
}
    
    
string Table::path_dir() const {
    return database.pathDir + name;
}