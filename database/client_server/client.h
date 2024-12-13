#ifndef DATABASE_CLIENT_H_GUARD
#define DATABASE_CLIENT_H_GUARD

#include "client_server_base.h"


struct Response {
    bool success;
    std::string data;
};


class DBClient: ClientServerBase {
public:
    DBClient (std::string const& ip, std::size_t port);
    ~DBClient() noexcept override;

    Response send_request (std::string const&);
};

#endif