#ifndef CLIENT_SERVER_BASE_H_GUARD
#define CLIENT_SERVER_BASE_H_GUARD

#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>


class ClientServerBase {
protected:
    ClientServerBase();

    sockaddr_in get_address (std::string const& ip, std::size_t port);

    void resize (std::size_t);
    
    void send (int, std::string const& mess);
    std::string receive(int);

    virtual ~ClientServerBase() noexcept;

protected:
    int sock;
    bool isConnected;
    
    std::size_t buffSize = 32;
    char* buff = nullptr;
};

#endif