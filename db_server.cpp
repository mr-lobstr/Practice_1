#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include "database/database.h"
#include "parser/parser.h"
using namespace std;
using namespace data_struct;

mutex mtx;
ofstream logFile ("server_log.txt");

void handleClient(int clientSocket, Database& database) {
    char buffer[8192];

    while (true) {
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytes <= 0 or StringView (buffer, bytes) == "exit"s){
            break;
        }

        {
            lock_guard<mutex> lock (mtx);
            logFile << "client request: " << StringView (buffer, bytes) << endl;
        }

        auto responce = database.execute_request ({buffer, bytes});
        auto size = to_string (responce.size());

        {
            lock_guard<mutex> lock (mtx);
            logFile << "server responce: " << responce << endl;
        }

        send(clientSocket, size.c_str(), size.size(), 0);
        bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        send(clientSocket, responce.c_str(), responce.size(), 0);
    }
    
    close(clientSocket);
}


int main (int argc, char** argv) {
    string pathDir, pathSchema;

    if (argc < 3) {
        pathDir = "./";
        pathSchema = "schema.json";
    } else {
        pathDir = argv[1];
        pathSchema = argv[2];
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7432);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 10);

    cout << "Server start\n";

    Database database(pathDir, pathSchema);

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);    
        thread clientThread(handleClient, clientSocket, ref(database));
        clientThread.detach();
    }

    close(serverSocket);
}