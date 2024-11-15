#include <iostream>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

size_t bufSize = 8192;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7432);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    char buffer[bufSize];

    while (true) {
        cout << ">> ";

        string command;
        getline(cin, command);

        if (command.size() == 0)
            continue;

        if (command == "exit") {
            send(clientSocket, command.c_str(), command.size(), 0);
            break;
        }
    
        send(clientSocket, command.c_str(), command.size(), 0);
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0) {
            cout << "server: " << string(buffer, bytesReceived) << endl;
        }
    }

    close(clientSocket);
}