#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "database/database.h"
#include "parser/parser.h"
using namespace std;
using namespace data_struct;

size_t bufSize = 8192;

string execute_request (Database& database, string const& strRequest) {
    Parser parser;
    parser.give_str (strRequest);
    RequestPtr request;

    try {
        request = parser.parse();
    } catch (exception const& e) {
        return "во время разбора запроса возникла ошибка:\n"s + e.what();
    }

    try {
        return request->execute(database);
    } catch (exception const& e) {
        return e.what();
    }
}

void handleClient(int clientSocket, Database& database) {
    char buffer[8192];

    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes <= 0 or StringView (buffer, bytes) == "exit"s){
            break;
        }

        auto result = execute_request (database, {buffer, bytes});
        send(clientSocket, result.c_str(), result.size(), 0);
    }
    
    close(clientSocket);
}


int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7432);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 10);

    cout << "Server start\n";

    Database database("./", "schema.json");

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);    
        thread clientThread(handleClient, clientSocket, ref(database));
        clientThread.detach();
    }

    close(serverSocket);
}