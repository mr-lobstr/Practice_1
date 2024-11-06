#include "cartesian_iterator.h"
#include "database.h"
using namespace std;
using namespace data_struct;


CartesianIterator::CartesianIterator (Database& db, TablesNames const& tablesNames)
    : database(db)
{
    tablesIters.reserve (tablesIters.size());

    for (auto& name : tablesNames) {
        tablesIters.emplace_back (name, database.get_table(name).begin());
    }
}


CartesianIterator::~CartesianIterator() noexcept {
    reset();
}


Table::Iterator const& CartesianIterator::operator[] (string const& tableName) const {
    auto it = algs::find_if (tablesIters.begin(), tablesIters.end(), [&] (auto& pair) {
        return pair.first == tableName;
    });

    if (it == tablesIters.end()) throw std::runtime_error (
        "таблица \'" + tableName + "\' отсутствует\n"
    );

    return it->second;
}


void CartesianIterator::operator++() {
    size_t cnt = 0;

    for (auto& [_, it] : tablesIters) {
        ++it;

        if (not it.is_end())
            break;

        ++cnt;
    }

    if (cnt != tablesIters.size()) {
        for (size_t i = 0; i < cnt; ++i) {
            auto& [tableName, it] = tablesIters[i];
            it = database.get_table (tableName).begin();
        }
    } else {
        isEnd = true;
    }
}


bool CartesianIterator::is_end() const noexcept {
    return isEnd;
}


void CartesianIterator::reset() noexcept {
    if (not isEnd) {
        for (auto& [_, it] : tablesIters) {
            it.reset();
        }

        isEnd = true;
    }
}