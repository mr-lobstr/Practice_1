#ifndef ITERATOR_WITH_CONDITION_H_GUARD
#define ITERATOR_WITH_CONDITION_H_GUARD

#include <string>
#include "database_fwd.h"
#include "cartesian_iterator.h"
#include "../data_struct/binary_tree.h"



class IteratorWithCondition {
public:
    IteratorWithCondition (Database&, TablesNames const&, Condition&, TMode);

    Table::Iterator const& operator[] (std::string const&) const;

    void operator++();
    void validate ();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    using OperIt = Condition::ConstBranchIt;

    bool condition_fulfilled() const;
    bool compute_condition (OperIt) const;
    StringView get_operand (StringView) const noexcept;
    bool equal (OperIt) const;

private:
    CartesianIterator iter;
    Condition& condition;
};

#endif