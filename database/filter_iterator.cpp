#include "filter_iterator.h"
using namespace std;
using namespace data_struct;

using IWC = FilterIterator;

IWC::FilterIterator (Database& db, TablesNames const& tNames, TableColumnPairs const& tcPairs, Condition const& cond)
    : iter (db, tNames, tcPairs)
    , condition (cond)
{
    validate();
}


Table::Iterator const& IWC::operator[] (string const& tableName) const {
    return iter[tableName];
}


string IWC::cross() const {
    return iter.cross();
}


void IWC::operator++() {
   ++iter;
    validate();
}


bool IWC::is_end() const noexcept {
    return iter.is_end();
}


void IWC::reset() noexcept {
    iter.reset();
}


void IWC::validate () {
    while (not is_end() and not condition_fulfilled()) {
        ++iter;
    }
}


StringView IWC::get_operand (StringView sv) const {
    if (sv.front() == '\'') {
        sv.shorten_left (1);
        sv.shorten_right (1);
        return sv;
    }

    auto tableColumn = split (sv, '.');
    auto table = tableColumn[0];
    auto column = tableColumn[1];
        
    return iter[table][column];
}


bool IWC::equal (OperIt it) const {
    auto& left = *it.left();
    auto& right = *it.right();

    return get_operand (left) == get_operand (right);
}


bool IWC::compute_condition (OperIt operIt) const {
    if (*operIt == "=") 
        return equal (operIt);

    if (*operIt == "AND") 
        return compute_condition (operIt.left())
           and compute_condition (operIt.right());

    if (*operIt == "OR") 
        return compute_condition (operIt.left())
            or compute_condition (operIt.right()); 
}


bool IWC::condition_fulfilled() const {
    if (condition.empty()) {
        return true;
    }

    return compute_condition (condition.root());
} 