#include <stdexcept>
#include <format>
#include "client_server_base.h"

using namespace std;

ClientServerBase::ClientServerBase()
    : sock (socket (AF_INET, SOCK_STREAM, 0))
    , buff (new char[buffSize])
{
    if (sock == -1) throw runtime_error (
        "не удалось создать новый сокет\n"
    );
}



sockaddr_in ClientServerBase::get_address (string const& ip, size_t port) {
    return {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {
            .s_addr = inet_addr (ip.c_str())
        }
    };
}


void ClientServerBase::resize (size_t newSize) {
    if (newSize > buffSize) {
        buffSize = newSize;
        delete (buff);
        buff = new char[buffSize];
    }
}


void ClientServerBase::send (int outSock, string const& mess) {
    auto bytes = ::send(outSock, mess.c_str(), mess.size(), 0);

    if (bytes == -1) throw runtime_error ( format (
        "не удалось отправить сообщение:\n{}", mess
    ));
}


string ClientServerBase::receive (int outSock) {
    auto bytes = recv(outSock, buff, buffSize, 0);

    if (bytes == -1) throw runtime_error (
        "не удалось получить сообщение"
    );

    return {buff, (size_t) bytes};
}


ClientServerBase::~ClientServerBase() noexcept {
    close (sock);
    delete (buff);
}