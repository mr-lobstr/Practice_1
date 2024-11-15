#ifndef PARSER_H_GUARD
#define PARSER_H_GUARD

#include <string>
#include "conditions_parser.h"
#include "request.h"
#include "../database/database_fwd.h"
#include "../data_struct/dynamic_array.h"


class Parser {
    using Tokens = ds::DynamicArray<StringView>;
    using TokenIter = Tokens::const_iterator;
    using TokenIter_ = decltype (std::declval<TokenIter>()++);

public:
    void give_str (std::string s);
    RequestPtr parse();

private:
    void tokenize();

    RequestPtr insert_parse();
    RequestPtr delete_parse();
    RequestPtr select_parse();

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