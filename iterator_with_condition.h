#ifndef ITERATOR_WITH_CONDITION_H_GUARD
#define ITERATOR_WITH_CONDITION_H_GUARD

#include <string>
#include "data_struct/dynamic_array.h"
#include "conditions_parser.h"
#include "cartesian_iterator.h"


class IteratorWithCondition {
public:
    IteratorWithCondition (Database&, ds::DynamicArray<std::string> const&, Conditions&);

    Table::Iterator const& operator[] (std::string const&) const;

    void next();
    bool is_end() const noexcept;
    void reset() noexcept;

private:
    void validate ();

    using OperIt = Conditions::ConstBranchIt;

    bool condition_fulfilled() const;
    bool compute_condition (OperIt) const;
    StringView get_operand (StringView) const noexcept;
    bool equal (OperIt) const;

private:
    CartesianIterator iter;
    Conditions& condition;
};

#endif