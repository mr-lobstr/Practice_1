#ifndef DELETE_ITERATOR_H_GUARD
#define DELETE_ITERATOR_H_GUARD

#include <string>
#include "../database/database_fwd.h"
#include "../table/table.h"
#include "../data_struct/binary_tree.h"


class DeleteIterator {
public:
    DeleteIterator (Database&, TableName const&, Condition const&);

    void operator++();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    void validate();
    void del();

    using OperIt = Condition::ConstBranchIt;

    bool condition_fulfilled() const;
    bool compute_condition (OperIt) const;
    SView get_operand (SView) const noexcept;
    bool equal (OperIt) const;

private:
    Table::Iterator iter;
    Condition condition;
};

#endif