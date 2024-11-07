#include "parser.h"
using namespace std;
using namespace data_struct;


void Parser::give_str (string s) {
    str = move (s);
}


Request* Parser::parse() {
    tokenize();

    throw_if (tokens.empty(),
        "невозможно разобрать пустое выражение\n"
    );

    auto first = *tokenIt++;

    if (first == "INSERT"s) {
        return insert_parse();
    } else if (first == "DELETE"s) {
        return delete_parse();
    } else if (first == "SELECT"s) {
        return select_parse();
    } else {
        throw_if (true,
            "неизвестная команда: " + first + "\n"
        );
    }
}


void Parser::tokenize() {
    tokens.resize(0);

    for (auto beg = str.begin(); beg != str.end(); ++beg) {       
        if (isspace (*beg)) {
            continue;
        } else if (*beg == '\'') {
            auto it = algs::find_if (beg+1, str.end(), [this] (char c) {
                return c == '\'';
            });

            if (it == str.end()) {
                tokens.emplace_back (beg, it);
                break;
            }
            
            tokens.emplace_back (beg, it + 1);
            beg = it;
            
        } else if (is_special (*beg)) {
            tokens.emplace_back (beg, beg + 1);
        } else {
            auto it = algs::find_if (beg, str.end(), [this] (char c) {
                return isspace (c) or is_special (c);
            });

            tokens.emplace_back (beg, it);
            beg = it - 1;
        }
    }

    tokenIt = tokens.begin();
}


Request* Parser::insert_parse() {
    auto pReq = new InsertRequest (move (str));

    expected_received ("INTO", tokenIt++);
    pReq->tableName =  *tokenIt++;

    expected_received ("VALUES", tokenIt++);
    expected_received ("(", tokenIt++);

    while (not token_check (")", tokenIt)) {
        pReq->row.push_back (get_value (*tokenIt++));

        if (not token_check (")", tokenIt)) {
            expected_received (",", tokenIt);
            ++tokenIt;
        }
    }

    return pReq;     
}


Request* Parser::delete_parse() {
    auto pReq = new DeleteRequest (move (str));;
    auto tablesNames = from_parse();

    throw_if (tablesNames.size() != 1,
        "ожидался один аргумент, получено " + to_string (tablesNames.size()) + "\n"
    );

    pReq->tableName = tablesNames.front();
    pReq->condition = where_parse();

    return pReq;
}


Request* Parser::select_parse() {
    auto pReq = new SelectRequest (move (str));

    pReq->tcPairs = select_parse_();
    pReq->tablesNames = from_parse();

    tables_names_check (pReq->tcPairs, pReq->tablesNames);

    if (tokenIt != tokens.end()) {
        auto pFReq = new FilterRequest (move (*pReq));
        pFReq->condition = where_parse();
        delete (pReq);

        return pFReq;
    }

    return pReq;
}


TablesNames Parser::from_parse() {
    auto isEnd = [this] {
        return tokenIt == tokens.cend() or *tokenIt == "WHERE"s;
    };

    TablesNames tablesNames;
    expected_received ("FROM", tokenIt++);
        
    while (not isEnd()) {
        tablesNames.push_back (*tokenIt++);

        if (not isEnd()) {
            expected_received (",", tokenIt);
            ++tokenIt;
        }
    }

    return tablesNames;
}


Conditions Parser::where_parse() {
    expected_received ("WHERE", tokenIt++);

    return ConditionParser(
        Tokens (tokenIt, tokens.cend())
    ).get_condition();
}


TableColumnPairs Parser::select_parse_() {
    auto isEnd = [this] {
        return tokenIt == tokens.cend() or *tokenIt == "FROM"s;
    };

    TableColumnPairs tcPairs;
        
    while (not isEnd()) {
        tcPairs.push_back (get_table_column (*tokenIt++));

        if (not isEnd()) {
            expected_received (",", tokenIt);
            ++tokenIt;
        }
    }

    return tcPairs;       
}


bool Parser::is_special (char c) const noexcept{
    return c == '('
        or c == ')'
        or c == ',';
}


bool Parser::token_check (string const& exp, TokenIter_ it) {
    throw_if (it == tokens.cend(),
        "выражение неполное: ожидалось \'" + exp + "\'\n"
    );

    return *it == exp;
}


void Parser::expected_received (string const& exp, TokenIter_ it) {
    throw_if (not token_check (exp, it),
        "ожидалось \'" + exp + "\', получено \'" + *it + "\'\n"
    ); 
}


StringView Parser::get_value (StringView value) {
    if (value.front() == '\'' and value.back() == '\'') {
        throw_if (algs::count (value.begin(), value.end(), ' ') != 0,
            "значение " + value + " содержит пробелы\n"
        );

        value.shorten_left (1);
        value.shorten_right (1);
        return value;
    }

    throw_if (true,
        "значение " + value + " должно находится в кавычках: \'\'\n"
    );
}


TableColumn Parser::get_table_column (StringView sv) {
    throw_if (algs::count (sv.begin(), sv.end(), '.') != 1,
        "имя таблицы и колонки должны быть разделены одной точкой\n"
    );

    auto pointIt = algs::find (sv.begin(), sv.end(), '.');
    auto tableNameSize = pointIt - sv.begin();
    auto columnNameSize = sv.end() - pointIt - 1;

    auto columnName = sv;
    sv.shorten_right (columnNameSize + 1);
    columnName.shorten_left (tableNameSize +1);

    return {sv, columnName};
}


void Parser::tables_names_check (TableColumnPairs const& tcPairs, TablesNames const& tablesNames) {
    HashSet<StringView> names (tablesNames.begin(), tablesNames.end());

    for (auto& [tableName, _] : tcPairs) {
            names.erase (tableName);
    }

    throw_if (not names.empty(),
        "имена таблиц в SELECT и в FROM не совпадают\n"
    );
}
