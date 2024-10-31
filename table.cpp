#include "table.h"
#include "table_state_guard.h"
#include "data_struct/string_view.h"

using namespace std;
using namespace data_struct;

Table::Table (string const& name_)
    : name (name_)
    , fm (*this)
{}

Table& Table::set_path (std::string const& path_) {
    pathDir = path_;
    return *this;
}


Table& Table::set_limit (std::size_t limit) {
    rowsLimit = limit;
    return *this;
}


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
