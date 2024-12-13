#ifndef CRYPTO_SERVER_H_GUARD
#define CRYPTO_SERVER_H_GUARD

#include <boost/beast/core.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../crypto_exchange/crypto.h"


class CryptoServer {
    using Request = boost::beast::http::request<boost::beast::http::string_body>;
    using Responce = boost::beast::http::response<boost::beast::http::string_body>;
    using Socket = boost::asio::ip::tcp::socket;

public:
    CryptoServer (std::string const&, std::size_t, std::string const&);
    void wait();

private:
    void client_handle (Socket);
    void session (Socket&, boost::beast::error_code&);
    void chech_error_code (boost::beast::error_code&);

    std::string get_user_key (Request const&);
    nlohmann::json request_processing (Socket&, Request&);

    void log (std::string const&);
    void log (std::ostream&, std::string const&);

private:
    WRSemaphore sema{};
    std::mutex mtxLog;

    boost::asio::io_context ioContext{};
    boost::asio::ip::tcp::acceptor acceptor;

    Configuration config;
};



#endif