#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "database/database.h"
#include "parser/parser.h"
using namespace std;
using namespace data_struct;


// #include "table/iterator_by_rows.cpp"
// #include "table/file_manager.cpp"
// #include "table/table.cpp"
// #include "data_struct/string_view.cpp"
// #include "table/table_state.cpp"
// #include "parser/conditions_parser.cpp"
// #include "parser/parser.cpp"
// #include "parser/request.cpp"
// #include "database/cartesian_iterator.cpp"
// #include "database/iterator_with_condition.cpp" 
// #include "database/database.cpp"


size_t bufSize = 8192;

void handleClient(int clientSocket, Database& database) {
    char buffer[8192];

    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes <= 0 or StringView (buffer, bytes) == "exit"s){
            break;
        }

        auto result = database.execute_request ({buffer, bytes});
        auto resultSize = to_string (result.size());


        send(clientSocket, resultSize.c_str(), resultSize.size(), 0);
        bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
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