#ifndef ITERATOR_WITH_CONDITION_H_GUARD
#define ITERATOR_WITH_CONDITION_H_GUARD

#include <string>
#include "database_fwd.h"
#include "cross_iterator.h"
#include "../data_struct/binary_tree.h"


class FilterIterator {
public:
    FilterIterator (Database&, TablesNames const&, TableColumnPairs const&, Condition const&);

    Table::Iterator const& operator[] (std::string const&) const;
    std::string cross() const;

    void operator++();
    void validate ();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    using OperIt = Condition::ConstBranchIt;

    bool condition_fulfilled() const;
    bool compute_condition (OperIt) const;
    StringView get_operand (StringView) const;
    bool equal (OperIt) const;

private:
    CrossIterator iter;
    Condition condition;
};

#endif
