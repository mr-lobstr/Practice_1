#include <string>
#include <iostream>
#include "database/database.h"
#include "parser/parser.h"

#include "table/iterator_by_rows.cpp"
#include "table/file_manager.cpp"
#include "table/table.cpp"
#include "data_struct/string_view.cpp"
#include "table/table_state_guard.cpp"
#include "parser/conditions_parser.cpp"
#include "parser/parser.cpp"
#include "parser/request.cpp"
#include "database/cartesian_iterator.cpp"
#include "database/iterator_with_condition.cpp"
#include "database/database.cpp"

using namespace std;
using namespace data_struct;

int main()
{
    Database database("./", "schema.json");
    Parser parser;
    string s;

    while (true) {
        cout << ">>> ";
        getline (cin, s);

        if (s == "exit") {
            break;
        }

        parser.give_str (s);
        Request* request;

        try {
            request = parser.parse();
        } catch (exception const& e) {
            cerr << "во время разбора запроса возникла ошибка:\n"
                 << e.what();
            continue;
        }

        try {
            request->execute(database);
        } catch (exception const& e) {
            cerr << e.what();
            continue;
        }

        delete (request);
    }
}