#include <iostream>
#include <string>
#include "./client_server/server.h"
using namespace std;

int argsCount = 0;
char** argV = nullptr;

void register_args (int argc, char** argv) {
    argsCount = argc;
    argV = argv;
}

string get_arg_or (int n, string const& def) {
    return (n < argsCount) ? argV[n] : def;
}

int main (int argc, char** argv) 
{
    register_args (argc, argv);

    string pathSchema = get_arg_or (1, "./schema.json");
    string pathDir = get_arg_or (2, "./");
    string ip = get_arg_or (3, "0.0.0.0");
    int port = stoi (get_arg_or (4, "8080"));

    Database database(pathDir, pathSchema);
    DBServer server (database, ip, port);
    cout << "Server start..." << endl;

    try {
        server.wait_requests();
    } catch (exception const& e) {
        cout << e.what() << endl;
    }
}