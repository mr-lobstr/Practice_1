#include <iostream>
#include <memory>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

class DBClient {
public:
    DBClient (string const& ip, size_t port)
        : socket_ (socket(AF_INET, SOCK_STREAM, 0))
        , buff (new char[buffSize])
    {    
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons (port);
        serverAddr.sin_addr.s_addr = inet_addr (ip.c_str());

        connect (socket_, (sockaddr*) &serverAddr, sizeof(serverAddr));
    }

    ~DBClient() noexcept {
        send (socket_, "exit", 4, 0);
        close (socket_);
        delete (buff);
    }

    string request_responce (string const& req) {
        send(socket_, req.c_str(), req.size(), 0);

        size_t received = recv(socket_, buff, buffSize, 0);
        auto size = stoll ({buff, received});

        if (buffSize < size) {
            delete (buff);
            buff = new char[size];
            buffSize = size;
        }

        send(socket_, "Ok", 2, 0);
        received = recv(socket_, buff, size, 0);

        return {buff, received};
    }

private:
    int socket_;
    char* buff;
    size_t buffSize = 32;

};

int main() {
    DBClient client ("0.0.0.0", 7432);

    while (true) {
        cout << ">> ";

        string req;
        getline(cin, req);

        if (req.size() == 0)
            continue;

        if (req == "exit") {
            break;
        }
    
        cout << "server: " << client.request_responce (req) << endl;
    }
}