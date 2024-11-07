#include <string>
#include <iostream>
#include "database/database.h"
#include "parser/parser.h"
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