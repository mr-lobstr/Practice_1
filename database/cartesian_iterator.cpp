#include "cartesian_iterator.h"
#include "database.h"
using namespace std;
using namespace data_struct;


CartesianIterator::CartesianIterator (Database& db, TablesNames const& tNames, TMode mode)
    : database(db)
{
    tablesIters.reserve (tablesIters.size());

    for (auto& tName : tNames) {
        tablesIters.push_back ({
            tName
          , db.tables[tName].make_iter (mode)
        });
    }
}


CartesianIterator::~CartesianIterator() noexcept {
    reset();
}


Table::Iterator const& CartesianIterator::operator[] (string const& tName) const {
    auto it = algs::find_if (tablesIters.begin(), tablesIters.end(), [&] (auto& pair) {
        return pair.first == tName;
    });

    if (it == tablesIters.end()) throw std::runtime_error (
        "таблица \'" + tName + "\' отсутствует\n"
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
            auto& [tName, it] = tablesIters[i];
            it.restart();
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