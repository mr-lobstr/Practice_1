#ifndef CRYPTO_H_GUARD
#define CRYPTO_H_GUARD

#include <string>
#include <map>
#include <vector>
#include "db_client.h"


class Crypto {
public:
    struct Lot;
    struct Pair;
    struct User;
    struct UserLot;
    struct Order;
    using Balance = std::map<int, double>;

public:
    Crypto (std::string const&, std::size_t, std::vector<std::string>);

    std::string post_user (std::string const&);
    int post_order (std::string const&, Order const&);

    std::vector<Lot> get_lots();
    std::vector<Pair> get_pairs();
    Balance get_balance (std::string const&);

    void delete_order (std::string const&, int);

public:
    template <typename... Args>
    std::string db_request (std::string const&, Args&&...);

    template <typename... Args>
    std::vector<Order> get_orders_where (std::string const&, Args&&...);

    template <typename... Args>
    std::vector<Lot> get_lots_where (std::string const&, Args&&...);

    template <typename... Args>
    std::vector<UserLot> get_user_lots_where (std::string const&, Args&&...);

    template <typename... Args>
    User get_user_where (std::string const&, Args&&...);

    bool is_init();
    void init (std::vector<std::string> const&);

    std::string add_lot (std::string const&);
    std::string add_user_lot (int, int, double);

    Pair get_pair (int);

    void change_balance (int, int, double);

    int deal (Order&, Order&);

    int add_order (Order const&);
    int closed_order (Order const&);
    void delete_order (Order const&);

    int user_verification (std::string const&);
    void check_repeat_username (std::string const&);
    void check_change_quantity (double, UserLot const&);
    Order check_and_get_user_order (int, int);

private:
    DBClient client;
};

#endif