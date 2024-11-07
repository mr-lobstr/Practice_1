#ifndef ITERATOR_WITH_CONDITION_H_GUARD
#define ITERATOR_WITH_CONDITION_H_GUARD

#include <string>
#include "cartesian_iterator.h"
#include "../data_struct/dynamic_array.h"
#include "../data_struct/binary_tree.h"

using Condition = ds::BinTree<std::string>;
using TableColumn = ds::Pair<StringView, StringView>;
using TableColumnPairs = ds::DynamicArray<TableColumn>;


class IteratorWithCondition {
public:
    IteratorWithCondition (Database&, TablesNames const&, Condition&);

    Table::Iterator const& operator[] (std::string const&) const;

    void operator++();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    using OperIt = Condition::ConstBranchIt;

    void validate ();

    bool condition_fulfilled() const;
    bool compute_condition (OperIt) const;
    StringView get_operand (StringView) const noexcept;
    bool equal (OperIt) const;

private:
    CartesianIterator iter;
    Condition& condition;
};

#endif