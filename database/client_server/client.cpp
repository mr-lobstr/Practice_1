#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "client.h"
using namespace std;


DBClient::DBClient (string const& ip, size_t port)
    : ClientServerBase()
{    
    auto address = get_address (ip, port);
    auto connection = connect (sock, (sockaddr*) &address, sizeof(address));

    if (connection == -1) throw runtime_error (
        "не удалось подключиться к серверу базы данных\n"
    );

    resize (32);
}


DBClient::~DBClient() noexcept {
    try {
        send (sock, "exit");
    } catch(...) {}
}


Response DBClient::send_request (string const& request) {
    send (sock, request);

    auto respSize = stoll (receive (sock));
    resize (respSize);
    send (sock, "Ok");
    
    auto responce = receive (sock);
    auto success = responce.starts_with("Ok");
    auto skip = 1 + (success ? 2 : 3);

    if (responce.size() > 3 and responce.back() == '\n') {
        responce.pop_back();
    }

    return {success, responce.substr (skip)};
}