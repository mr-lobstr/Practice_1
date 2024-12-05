#ifndef DATABASE_CLIENT_H_GUARD
#define DATABASE_CLIENT_H_GUARD

#include <string>


struct Response {
    bool success;
    std::string data;
};


class DBClient {
public:
    DBClient (std::string const& ip, std::size_t port);
    ~DBClient() noexcept;

    Response send_request (std::string const&);

private:
    int socket_;
    bool isConnected;
    
    std::size_t buffSize = 32;
    char* buff = nullptr;
};

#endif