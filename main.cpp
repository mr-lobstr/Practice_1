#include <string>
#include <iostream>
#include "database.h"

#include "table/iterator_by_rows.cpp"
#include "table/file_manager.cpp"
#include "table/table.cpp"
#include "data_struct/string_view.cpp"
#include "table/table_state_guard.cpp"
#include "conditions_parser.cpp"
#include "cartesian_iterator.cpp"
#include "iterator_with_condition.cpp"
#include "database.cpp"

using namespace std;
using namespace data_struct;

int main()
{
    Database db("./", "schema.json");
    int _;
    // cin >> _;
    string s = "таблица1";
    DynamicArray<StringView> dsv = {"1,"s, "2,"s, "3,"s, "4,"s};
    db.insert (s, dsv);    

    string condS = " таблица1.колонка1 = 'Ababua' OR таблица2.колонка2 = 'aa' ";
    auto cond = ConditionParser (condS).get_condition();

    DynamicArray<string> d = {"таблица1"};
    db.filter (
        {"таблица1", "таблица2"}
      , {
            {"таблица1"s, "колонка1"s}
          , {"таблица1"s, "колонка2"s}
          , {"таблица1"s, "колонка3"s}
          , {"таблица1"s, "колонка4"s}
          , {"таблица2"s, "колонка1"s}
          , {"таблица2"s, "колонка2"s}        
        }
      , cond
    );
    // IteratorWithCondition it (db, d, cond);

    // while (not it.is_end()) {
    //     // string t1 = "таблица1", t2 = "таблица2", c1 = "колонка1";

    //     // cout << it[t1]["таблица1_pk"]<< " " <<it[t1][c1] << " ";
    //     // cout << it[t2]["таблица2_pk"]<< " " << it[t2][c1] << endl;

    //     it["таблица1"].erase();
    //     ++it;
    // }
}