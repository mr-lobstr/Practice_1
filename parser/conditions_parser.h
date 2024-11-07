#ifndef CONDITION_PARSER_H_GUARD
#define CONDITION_PARSER_H_GUARD

#include "../data_struct/string_view.h"
#include "../data_struct/binary_tree.h"
#include "../data_struct/my_algorithm.h"
namespace ds = data_struct;

using Conditions = ds::BinTree<std::string>;

class ConditionParser {
public: 
    explicit
    ConditionParser (ds::DynamicArray<StringView>);

    Conditions get_condition();
    bool empty() const noexcept;

private:
    Conditions get_next_token();

    bool is_eq (std::string const&) const noexcept;
    bool is_operator (std::string const&) const noexcept;
    bool is_operand (std::string const&) const noexcept;
    int priority (std::string const&) const noexcept;

private:
    ds::DynamicArray<StringView> words;
};

void throw_if (bool, std::string const&); 

#endif