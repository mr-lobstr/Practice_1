#include <format>
#include <iostream>
#include <thread>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "server.h"
#include "../utility/utility.h"
using namespace std;

DBServer::DBServer (Database& db, string const& ip, size_t port)
    : ClientServerBase()
    , database (db)
    , logFile ("server_log.txt", ios::ate)
{    
    auto address = get_address (ip, port);
    bind(sock, (struct sockaddr*) &address, sizeof(address));
    auto success = listen(sock, maxConnections);
    
    if (success == -1) throw runtime_error (format (
        "не удалось подготовиться к приему {} подключений\n", maxConnections
    ));

    resize (minBuffSize);
    print_start (address);
}


void DBServer::print_start (sockaddr_in address) {
    logFile << "Server start..." << "\n";

    char ipstr[INET_ADDRSTRLEN] = {};

    socklen_t len = sizeof (address);
    getsockname (sock, (sockaddr*) &address, &len);
    inet_ntop (AF_INET, &(address.sin_addr), ipstr, sizeof(ipstr));

    logFile << "IP address: " << ipstr << endl
            << "Port: " << ntohs(address.sin_port) << endl;
}


DBServer::~DBServer() noexcept {
    logFile << "Server stop." << "\n";
}


void DBServer::wait_command() {
    string command;

    while (cin >> command) {
        if (command == "exit") {
            lock_guard<mutex> lock (mtxForCommand);
            exit = true;
            return;
        }
    }
}


bool DBServer::check_exit() {
    lock_guard<mutex> lock (mtxForClient)
                    , lock2 (mtxForCommand);
    
    return clientsCnt == 0 and exit;
}


void DBServer::wait_requests() {
    thread ([this] {
        wait_command();
    }).detach();

    fd_set readfds;
    timeval timeout {
        .tv_sec = 5
    };

    logFile << flush;
    wait_loop (readfds, timeout);
}


void DBServer::wait_loop (fd_set& readfds, timeval& timeout) {
    while (true) {
        int readySockets = 0;

        while (readySockets == 0) {
            FD_ZERO (&readfds);
            FD_SET (sock, &readfds);

            readySockets = select (20, &readfds, nullptr, nullptr, &timeout);

            if (readySockets == 0 and check_exit()) {
                return;
            }
        }
        
        auto clientSock = accept(sock, nullptr, nullptr);
         
        thread ([&, this] {
            try {
                request_processing (clientSock, database);
            } catch (exception const& e) {
                print_log ("error:\n"s + e.what());
            }
        }).detach();
    }
}


DBServer::PGuard::PGuard (int sock, size_t& cnt, std::mutex& mtx_)
    : clientSock (sock)
    , clientsCnt (cnt)
    , mtx (mtx_)
{
    lock_guard<mutex> lock (mtx);
    ++clientsCnt;
}


DBServer::PGuard::~PGuard() noexcept {
    lock_guard<mutex> lock (mtx);
    --clientsCnt;
    try {
        close(clientSock);
    } catch (...) {}
}


void DBServer::request_processing (int clientSock, Database& db) {
    PGuard guard (clientSock, clientsCnt, mtxForClient);

    while (true) {
        auto request = receive (clientSock);
        print_log (format (
            "client {} request:\n{}", clientSock, request
        ));

        if (request == "exit") {
            break;
        }

        auto responce = database.execute_request (request);
        print_log ("server responce:\n" + responce);

        auto respSize = to_string (responce.size());
        send (clientSock, respSize);
        auto recv = receive (clientSock);

        if (recv != "Ok") throw runtime_error ( format (
            "ошибка со стороны клиента: {},\n"
            "при попытке передать сообщение {}\n",
            recv, responce
        ));

        send (clientSock, responce);
    }
}


void DBServer::print_log (string const& mess) {
    lock_guard<mutex> lock (mtxForLog);

    logFile << string (20, '-') << "\n"
            << "[" << current_time_str() << "]\n"
            << mess << "\n" 
            << string (20, '-') << "\n\n";
}
