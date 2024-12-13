#ifndef DATABASE_SERVER_H_GUARD
#define DATABASE_SERVER_H_GUARD

#include <fstream>
#include <mutex>
#include "client_server_base.h"
#include "../database/database.h"


class DBServer: ClientServerBase {
public:
    DBServer (Database&, std::string const& ip, std::size_t port);
    ~DBServer() noexcept override;

    void wait_requests();

private:
    void wait_command();
    bool check_exit();
    void wait_loop (fd_set&, timeval&);

    class PGuard;
    void request_processing (int, Database&);

    void print_log (std::string const&);
    void print_start (sockaddr_in);

private:
    static constexpr std::size_t maxConnections = 10;
    static constexpr std::size_t minBuffSize = 8192;

private:
    Database& database;

    std::ofstream logFile;
    std::mutex mtxForLog;
    std::mutex mtxForCommand;
    std::mutex mtxForClient;

    std::size_t clientsCnt = 0;
    bool exit = false;
};


class DBServer::PGuard {
public:
    PGuard (int, std::size_t&, std::mutex&);
    ~PGuard() noexcept;

private:
    int clientSock;
    std::size_t& clientsCnt;
    std::mutex& mtx;
};

#endif