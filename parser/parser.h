#ifndef PARSER_H_GUARD
#define PARSER_H_GUARD

#include <string>
#include "../database/request_base.h"
#include "../database/database_fwd.h"
#include "../data_struct/dynamic_array.h"


class Parser {
    using Tokens = ds::DynamicArray<std::string>;
    using TokenIter = Tokens::const_iterator;
    using TokenIter_ = decltype (std::declval<TokenIter>()++);

public:
    void give_str (std::string s);
    RequestPtr parse (std::string const&);

private:
    void tokenize (std::string const&);

    RequestPtr insert_parse();
    RequestPtr delete_parse();
    RequestPtr select_parse();
    RequestPtr add_table_parse();

    TablesNames from_parse();
    Condition where_parse();
    TableColumnPairs select_parse_();

private:
    bool is_special (char c) const noexcept;
    bool token_check (std::string const& exp, TokenIter_ it);
    void expected_received (std::string const& exp, TokenIter_ it);
    StringView get_value (StringView value);
    TableColumn get_table_column (StringView sv);
    void tables_names_check (TableColumnPairs const& tcPairs, TablesNames const& tablesNames);

private:
    Tokens tokens{};
    TokenIter tokenIt{};
};


#endif