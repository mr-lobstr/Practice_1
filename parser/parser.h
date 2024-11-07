#ifndef PARSER_H_GUARD
#define PARSER_H_GUARD

#include <string>
#include "conditions_parser.h"
#include "request.h"
#include "../data_struct/hash_set.h"
#include "../data_struct/dynamic_array.h"
#include "../data_struct/string_view.h"

namespace ds = data_struct;

using TableColumn = ds::Pair<StringView, StringView>;
using TableColumnPairs = ds::DynamicArray<TableColumn>;
using TablesNames = ds::DynamicArray<std::string>;

class Parser {
    using Tokens = ds::DynamicArray<StringView>;
    using TokenIter = Tokens::const_iterator;
    using TokenIter_ = decltype (std::declval<TokenIter>()++);

public:
    void give_str (std::string s);
    Request* parse();

private:
    void tokenize();

    Request* insert_parse();
    Request* delete_parse();
    Request* select_parse();

    TablesNames from_parse();
    Conditions where_parse();
    TableColumnPairs select_parse_();

private:
    bool is_special (char c) const noexcept;
    bool token_check (std::string const& exp, TokenIter_ it);
    void expected_received (std::string const& exp, TokenIter_ it);
    StringView get_value (StringView value);
    TableColumn get_table_column (StringView sv);
    void tables_names_check (TableColumnPairs const& tcPairs, TablesNames const& tablesNames);

private:
    std::string str{};
    Tokens tokens{};
    TokenIter tokenIt{};
};


#endif