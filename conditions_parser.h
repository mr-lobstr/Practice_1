#ifndef CONDITION_PARSER_H_GUARD
#define CONDITION_PARSER_H_GUARD

#include "data_struct/string_view.h"
#include "data_struct/binary_tree.h"
#include "data_struct/list.h"
#include "data_struct/my_algorithm.h"


using Conditions = data_struct::BinTree<std::string>;


class ConditionParser {
public: 
    explicit
    ConditionParser (std::string);

    Conditions get_condition();

private:
    bool empty() const noexcept;

    Conditions get_next_token();


    bool is_eq (StringView const&) const noexcept;
    bool is_operator (StringView const&) const noexcept;
    bool is_operand (StringView const&) const noexcept;
    int priority (StringView const&) const noexcept;

private:
    std::string str;
    data_struct::List<StringView> words;
};

#endif