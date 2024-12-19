#include <iostream>
#include <string>
#include "server.h"
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

    string pathConfig = get_arg_or (1, "../example/crypto_test/config.json");
    string ip = get_arg_or (2, "0.0.0.0");
    int port = stoi (get_arg_or (3, "8081"));

    CryptoServer server (ip, port, pathConfig);
    cout << "Crypto server start..." << endl;
    
    try {
        server.wait();
    } catch (exception const& e) {
        cout << e.what() << endl;
    }
}