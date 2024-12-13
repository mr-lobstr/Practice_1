#include "delete_iterator.h"
#include "../database/database.h"
#include "../data_struct/dynamic_array.h"
#include "../utility/utility.h"

using namespace std;
using namespace data_struct;

using DI = DeleteIterator;

DI::DeleteIterator (Database& db, TableName const& tName, Condition const& cond)
    : iter (db.tables[tName].make_iter(TMode::writing))
    , condition (cond)
{
    validate();
    del();
}


void DI::operator++() {
    validate();
    del();
}


bool DI::is_end() const noexcept {
    return iter.is_end();
}


void DI::reset() noexcept {
    iter.reset();
}


void DI::validate () {
    while (not is_end() and not condition_fulfilled()) {
        ++iter;
    }
}


void DI::del() {
    if (not is_end()) {
        iter.erase();
    }
}


SView DI::get_operand (SView sv) const noexcept {
    if (sv.front() == '\'') {
        sv.shorten_left (1);
        sv.shorten_right (1);
        return sv;
    }

    auto tableColumn = sv_split (sv, '.');
    auto table = tableColumn[0];
    auto column = tableColumn[1];
        
    return iter[column];
}


bool DI::equal (OperIt it) const {
    auto& left = *it.left();
    auto& right = *it.right();

    return get_operand (left) == get_operand (right);
}


bool DI::compute_condition (OperIt operIt) const {
    if (*operIt == "=") 
        return equal (operIt);

    if (*operIt == "AND") 
        return compute_condition (operIt.left())
           and compute_condition (operIt.right());

    if (*operIt == "OR") 
        return compute_condition (operIt.left())
            or compute_condition (operIt.right()); 
}


bool DI::condition_fulfilled() const {
    return compute_condition (condition.root());
} 