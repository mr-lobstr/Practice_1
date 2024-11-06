#include "conditions_parser.h"
using namespace std;
using namespace data_struct;

void throw_if (bool errorCondition, std::string const& errorMessage) {
    if (errorCondition) throw std::runtime_error (
        errorMessage
    );
}

ConditionParser::ConditionParser (string str_)
    : str (std::move (str_))
{
    auto jj = split_into_words (str);
    for (auto sv : jj) {
        words.push_back (sv);
    }
}


template <typename Check>
string check_and_get_first (DynamicArray<string>& words, Check check) {
    string mess, first = words.front();
    bool condition;

    check (first, condition, mess);
    if (not condition)
        throw std::runtime_error (mess);

    words.erase (words.begin());
    return first;
}


bool ConditionParser::empty() const noexcept {
    return words.empty();
}


Conditions ConditionParser::get_next_token() {
    throw_if (empty(), 
        "невозможно разобрать пустую строку\n"
    );

    auto first = check_and_get_first (words,
        [this] (auto& f, auto& cond, auto& mess) {
            cond = (is_operand(f) or is_operator(f)) and not is_eq(f);
                
            mess = is_eq(f)
                 ? "слева от = требуется \'строка\' либо таблица.колонка\n"
                 : "\"" + (string) f + "\" не является ни операндом, ни оператором\n";
        }
    );

    Conditions ret;
    ret.push_root (Tilt::to_left, first);

    if (is_operator (first))
        return ret;

    check_and_get_first (words,
        [&, this] (auto& f, auto& cond, auto& mess) {
            cond = not empty() and is_eq(f);
            mess = "справа от " + first + " требуется оператор =\n";
        }
    );

    auto second = check_and_get_first (words,
        [this] (auto& f, auto& cond, auto& mess) {
            cond = not empty() and is_operand(f);
            mess = "справа от = требуется \'строка\' либо таблица.колонка\n";
        }
    );

    ret.push_root (Tilt::to_left, "=");
    ret.insert (Tilt::to_left, ret.root().right(), second);
    
    return ret;
}


Conditions ConditionParser::get_condition() {
    Conditions tree;
    bool isEqExpr = true;

    tree.add_subtree (tree.root(), get_next_token().root());

    while (not empty()) {
        auto subTree = get_next_token();
        auto tokenIt = subTree.root();
        auto it = tree.root();
        isEqExpr = not isEqExpr;

        while (not it.is_end() and priority (*tokenIt) > priority (*it)) {
            it.to_right();
        }

        throw_if (isEqExpr and *tokenIt != "=",
            "ожидалось выражение с =, а не " + *tokenIt + "\n"
        );
        
        if (not it.is_end()) {            
            subTree.add_subtree (subTree.root().left(), it);
        }
        tree.add_subtree (it, subTree.root());
    }

        return tree;
}


bool ConditionParser::is_eq (string const& word) const noexcept {
    return word == "=";
}


bool ConditionParser::is_operator (string const& word) const noexcept {
    return word == "OR"
        or word == "AND"
        or word == "=";
}


bool ConditionParser::is_operand (string const& word) const noexcept {
    if (word.front() == '\'' and word.back() == '\'')
        return true;

    auto it = algs::find (word.begin(), word.end(), '.');
    return it != word.end();
}

int ConditionParser::priority (string const& op) const noexcept {
    return op == "="   ? 3
         : op == "AND" ? 2
         : op == "OR"  ? 1
         : 0;
}
