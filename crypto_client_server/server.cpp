#include <iostream>
#include <thread>
#include <sstream>
#include <chrono>

#include "server.h"
#include "../database/utility/utility.h"

using namespace std;
using namespace chrono;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace sys = boost::system;
namespace ip = net::ip;

using tcp = ip::tcp;
using json = nlohmann::json;


CryptoServer::CryptoServer (string const& ip, size_t port, string const& configFileName)
    : acceptor (
        ioContext,
        {
            ip::make_address (ip),
            ip::port_type (port)
        }
    )
    , config (configFileName)
{
    ioContext.run();
}


void CryptoServer::wait() {
    while (true) {
        tcp::socket socket (ioContext);
        acceptor.accept (socket);
        
        thread ([this, sock = move (socket)] mutable {
            client_handle (move (sock));
        }).detach();
    }
}


void CryptoServer::client_handle (tcp::socket socket) {
    log ("Соединение установленно");
    beast::error_code ec{};
    
    while (ec != net::error::eof) {
        try {
            session (socket, ec);
            chech_error_code (ec);
        } catch (exception const& e) {
            log (cerr, e.what());
            return;
        }
    }

    log ("Соединение закрыто");
}


void CryptoServer::chech_error_code (beast::error_code& ec) {
    if (not ec or ec == net::error::eof)
        return;

    if (ec == net::error::broken_pipe) throw runtime_error (
        "соединение разорванно\n"
    );

    throw runtime_error (ec.message());
}


void CryptoServer::session (tcp::socket& socket, beast::error_code& ec) {
    Request request;
    beast::flat_buffer buffer;
    http::read (socket, buffer, request, ec); if (ec) return;

    string body, contentType;
    http::status status;
    
    try {
        body = request_processing (socket, request).dump (4);
        status = http::status::ok;
        contentType = "application/json";
    } catch (exception const& e) {
        body = format ("Ошибка при обработке запроса:\n{}", e.what());
        status = http::status::internal_server_error;
        contentType = "text/plain";
    }
    
    Responce responce {status, request.version(), body};
    responce.set(http::field::content_type, contentType);
    responce.prepare_payload();
    http::write (socket, responce);
}


string CryptoServer::get_user_key (Request const& request) {
    return request.at ("X-USER-KEY");
}


#include "handlers.cpp"


json CryptoServer::request_processing (tcp::socket& socket, Request& request) {
    Crypto crypto (config, sema);

    auto target = request.target();
    auto args = json();

    if (not request.body().empty()) {
        args = json::parse (request.body());
    }

    switch (request.method())
    {
    using http::verb;
    
    case verb::post:
             if (target == "/user")     return post_user (crypto, args);
        else if (target == "/order")    return post_order (crypto, args, get_user_key (request));
        break;

    case verb::get:
             if (target == "/order")    return get_order (crypto);
        else if (target == "/lot")      return get_lot (crypto);
        else if (target == "/pair")     return get_pair (crypto);
        else if (target == "/balance")  return get_balance (crypto, get_user_key (request));
        break;

    case verb::delete_:
             if (target == "/order")    return delete_order (crypto, args, get_user_key (request));
        break;
    }

    throw runtime_error (
        (stringstream{} << "неизвестный запрос:\n" << request).str()
    );
}


void CryptoServer::log (string const& mess) {
    log (cout, mess);
}


void CryptoServer::log (ostream& out, string const& mess) {
    lock_guard<mutex> lock (mtxLog);
    out << "[" << current_time_str() << "]" << "\n";
    out << mess << "\n";
}