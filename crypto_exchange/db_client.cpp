#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "db_client.h"
using namespace std;


DBClient::DBClient (string const& ip, size_t port)
    : socket_ (socket(AF_INET, SOCK_STREAM, 0))
{    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons (port);
    serverAddr.sin_addr.s_addr = inet_addr (ip.c_str());

    auto con = connect (socket_, (sockaddr*) &serverAddr, sizeof(serverAddr));

    if (con != 0) throw runtime_error ("888");

    buff = new char[buffSize];
}


DBClient::~DBClient() noexcept {
    send (socket_, "exit", 4, 0);
    close (socket_);
    delete (buff);
}


Response DBClient::send_request (string const& req) {
    send(socket_, req.c_str(), req.size(), 0);

    size_t received = recv(socket_, buff, buffSize, 0);
    auto size = stoll ({buff, received});

    if (buffSize < size) {
        buffSize = size;
        delete (buff);
        buff = new char[buffSize];
    }

    send(socket_, "Ok", 2, 0);
    received = recv(socket_, buff, buffSize, 0);

    bool success = ("Ok"s == string (buff, 2));
    auto data = buff + 1 +(success ? 2 : 3);
    auto dataSize = received - 1 - (success ? 2 : 3);

    if (dataSize != 0 and data[dataSize-1] == '\n') {
        --dataSize;
    }

    return {success, {data, dataSize}};
}