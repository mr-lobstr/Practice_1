#include "cross_iterator.h"
#include "../database/database.h"

using namespace std;
using namespace data_struct;


CrossIterator::CrossIterator (Database& db, TablesNames const& tNames, TableColumnPairs const& tcPairs_)
    : database (db)
    , tcPairs (tcPairs_)
{
    tablesIters.reserve (tcPairs.size());

    for (auto& table : tNames) {
        tablesIters.push_back ({
            table
          , db.tables[table].make_iter (TMode::reading)
        });
    }
}


CrossIterator::~CrossIterator() noexcept {
    reset();
}


Table::Iterator const& CrossIterator::operator[] (string const& tName) const {
    auto it = algs::find_if (tablesIters.begin(), tablesIters.end(), [&] (auto& tci) {
        return tci.table == tName;
    });

    if (it == tablesIters.end()) throw std::runtime_error (
        "таблица \'" + tName + "\' отсутствует\n"
    );

    return it->iter;
}


string CrossIterator::cross() const {
    bool isFirst = true;
    string result;

    for (auto& [table, column] : tcPairs) {
        auto& self = *this;
        result += (isFirst ? ""s : ","s) + self[table][column];
        isFirst = false;
    }
        
    return result;
}


void CrossIterator::operator++() {
    size_t cnt = 0;

    for (auto& [_, it] : tablesIters) {
        if (it.is_end()) {
            isEnd = true;
            return;
        }

        ++it;

        if (not it.is_end())
            break;

        ++cnt;
    }

    if (cnt != tablesIters.size()) {
        for (size_t i = 0; i < cnt; ++i) {
            auto& [_, it] = tablesIters[i];
            it.restart();
        }
    } else {
        isEnd = true;
    }
}


bool CrossIterator::is_end() const noexcept {
    return isEnd;
}


void CrossIterator::reset() noexcept {
    if (not isEnd) {
        for (auto& [_, it] : tablesIters) {
            it.reset();
        }

        isEnd = true;
    }
}