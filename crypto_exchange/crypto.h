#ifndef CRYPTO_H_GUARD
#define CRYPTO_H_GUARD

#include <map>
#include <string>
#include <vector>
#include "write_read_semaphore.h"
#include "crypto_elements.h"
#include "../database/client_server/client.h"


class Configuration {
    friend class Crypto;
public:
    explicit
    Configuration (std::string const&);

private:
    std::string ip{};
    std::size_t port{};
    std::vector<std::string> lots{};
};


class Crypto {
public:
    Crypto (Configuration const&, WRSemaphore&);

    Balance get_balance (std::string const&);
    template <typename T> auto get();

    std::string post_user (std::string const&);
    int post_order (std::string const&, Order const&);
    void delete_order (std::string const&, int);

private:
    std::string db_request (std::string const&);
    
    template <typename T, typename... Args> int add (Args&&...);
    template <typename T> T get_one (std::string const&);
    template <typename T> std::vector<T> get_all();
    template <typename T> std::vector<T> get_parse (std::string const&);

private:
    bool is_init();
    void init (std::vector<std::string> const&);

    void change_balance (int, int, double);

    int create_order (Order const&);
    int deal (Order&, Order&);
    int closed_order (Order const&);
    void delete_order (Order const&);

private:
    int user_verification (std::string const&);
    void check_repeat_username (std::string const&);
    void check_change_quantity (double, UserLot const&);
    Order check_and_get_user_order (int, int);

private:
    DBClient client;
    WRSemaphore& sema;
};


void to_json (nlohmann::json&, Balance const&);


template <typename T>
auto Crypto::get() {
    WriteLock lock (sema);
    return get_all<T>();
}


template <typename T, typename... Args>
int Crypto::add (Args&&... args) {
    auto request = T::add (std::forward<Args> (args)...);
    return std::stoi (db_request (request));
}


template <typename T>
std::vector<T> Crypto::get_parse (std::string const& request) {
    return parse<T> (db_request (request));
}


template <typename T>
T Crypto::get_one (std::string const& request) {
    auto ts = get_parse<T> (request);

    if (ts.size() != 1) throw std::invalid_argument (
        std::format ("запрос {} должен вернуть одно значение", request)
    );

    return ts.front();
}


template <typename T>
std::vector<T> Crypto::get_all() {
    return get_parse<T> (T::get_all());
}

#endif