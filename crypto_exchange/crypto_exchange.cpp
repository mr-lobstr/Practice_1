#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <format>
#include "crypto_exchange.h"
using namespace std;
using namespace chrono;

size_t rnd (size_t a = 0, size_t b = INT32_MAX) {
    static auto now = system_clock::now().time_since_epoch().count();
    static default_random_engine generator(now);
    static uniform_int_distribution<size_t> distribution(0, UINT64_MAX);

    return a + distribution(generator) % (b - a);
}


string random_string (size_t length) {
    string rndStr ('_', length);

    for (auto& symb : rndStr) {
        symb = rnd ('0', 'z');

        if (symb == '\'' or symb == ' ' or symb == ',')
           continue;
    }

    return rndStr;
}


template <typename... Args>
string Crypto::db_request (string const& fmt, Args&&... args) {
    string request;

    if constexpr (sizeof...(args) != 0) {
        request = vformat(fmt, make_format_args(args...));
    } else {
        request = fmt;
    }

    auto [success, responce] = client.send_request (request);

    if (not success) throw runtime_error (
        responce
    );

    return responce;
}


#define RETURN(func) {                                                      \
    if constexpr (sizeof...(args) != 0) {                                   \
        return func(db_request (select + where, forward<Args>(args)...));   \
    } else {                                                                \
        return func(db_request (select + where));                           \
    }                                                                       \
}


template <typename... Args>
vector<Crypto::Order> Crypto::get_orders_where (string const& where, Args&&... args) {
    auto select = 
        "SELECT  order.order_pk, "
                "order.user_id, "
                "order.pair_id, "
                "order.quantity, "
                "order.price, "
                "order.type, "
                "order.closed "
        "FROM order ";

    RETURN(parse<Order>);
}


template <typename... Args>
Crypto::User Crypto::get_user_where (string const& where, Args&&... args) {
    auto select = "SELECT user.user_pk, user.name, user.key FROM user ";

    RETURN(from_str<User>);
}


template <typename... Args>
vector<Crypto::Lot> Crypto::get_lots_where (string const& where, Args&&... args) {
    auto select = "SELECT lot.lot_pk, lot.name FROM lot ";

    RETURN (parse<Lot>);
}


template <typename... Args>
vector<Crypto::UserLot> Crypto::get_user_lots_where (string const& where, Args&&... args) {
    auto select =
        "SELECT  user_lot.user_lot_pk, "
                "user_lot.user_id, "
                "user_lot.lot_id, "
                "user_lot.quantity "
        "FROM user_lot ";

    RETURN (parse<UserLot>);
}


bool Crypto::is_init() {
    auto responce = db_request ("SELECT state.is_init FROM state");
    return responce == "init";
}


void Crypto::init (vector<string> const& lots) {
    db_request ("INSERT INTO state VALUES ('init', '1000')");

    vector<string> lotsId;

    for (auto& lot : lots) {
        auto lotId = add_lot (lot);
        lotsId.push_back (lotId);
    }

    for (size_t i = 0; i < lotsId.size(); ++i) {
        for (size_t j = i + 1; j < lotsId.size(); ++j) {
            db_request ("INSERT INTO pair VALUES ('{}', '{}')", lotsId[i], lotsId[j]);
        }
    }
}


string Crypto::add_lot (string const& lot) {
    return db_request ("INSERT INTO lot VALUES ('{}')", lot);
}


string Crypto::add_user_lot (int userId, int lotId, double quantity) {
    return db_request (
        "INSERT INTO user_lot VALUES ('{}', '{}', '{}')", userId, lotId, quantity
    );
}


string Crypto::post_user (string const& userName) {
    check_repeat_username (userName);
    auto userKey = random_string (50);
    
    auto userId = db_request ("INSERT INTO user VALUES ('{}', '{}')", userName, userKey);
    auto quantity = db_request ("SELECT state.default_quantity FROM state");

    for (auto& [lotId, _] : get_lots()) {
        add_user_lot (stoi (userId), lotId, stoi (quantity));
    }

    return userKey;
}


bool is_profit (Crypto::Order const& my, Crypto::Order const& other) {
    return my.type == "sell"
         ? my.price <= other.price
         : my.price >= other.price;
}


void Crypto::change_balance (int userId, int lotId, double sum) {
    auto userLot = get_user_lots_where (
        "WHERE user_lot.user_id = '{}' AND user_lot.lot_id = '{}'",
        userId, lotId
    ).front();

    check_change_quantity (sum, userLot);
    userLot.quantity += sum;

    db_request ("DELETE FROM user_lot WHERE user_lot.user_lot_pk = '{}'", userLot.id);
    add_user_lot (userId, lotId, userLot.quantity);
}


string closed_time() {
    using namespace chrono;

    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    
    tm tm = *localtime(&t);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;

    stringstream ss;
    ss << setfill('0') << setw(3) << ms << ":"
       << setfill('0') << setw(2) << tm.tm_sec << ":"
       << setfill('0') << setw(2) << tm.tm_min << ":"
       << setfill('0') << setw(2) << tm.tm_hour
       << put_time(localtime(&t), " %d-%m-%Y");

    return ss.str();
}


int Crypto::add_order (Order const& order) {
    auto resp = db_request (
        "INSERT INTO order "
        "VALUES ('{}', '{}', '{}', '{}', '{}', '{}')",
        order.userId
      , order.pairId
      , order.quantity
      , order.price
      , order.type
      , order.closed
    );

    auto [_, lot1, lot2] = get_pair (order.pairId);
    auto lot = (order.type == "sell" ? lot1 : lot2);
    auto sum = (order.type == "sell" ? 1 : order.price) * order.quantity;
    
    change_balance (order.userId, lot, -sum);

    return stoi (resp);
}


int Crypto::closed_order (Order const& order) {
    auto [_, lot1, lot2] = get_pair (order.pairId);
    auto lot = (order.type == "sell" ? lot2 : lot1);
    auto sum = (order.type == "sell" ? order.price : 1) * order.quantity;

    change_balance (order.userId, lot, +sum);

    return add_order (order);
}

Crypto::Order order_separate (Crypto::Order& order, double quantity) {
    auto newOrder = order;
    order.quantity -= quantity;
    newOrder.quantity = quantity;
    
    return newOrder;
}


int Crypto::deal (Order& my_, Order& other_) {
    delete_order (other_);

    auto quantity = min (my_.quantity, other_.quantity);
    auto my = order_separate (my_, quantity);
    auto other = order_separate (other_, quantity);

    my.closed = other.closed = closed_time();
    other.price = my.price;

    closed_order (other);
    auto id = closed_order (my);

    if (other_.quantity != 0) {
        add_order (other_);
    }

    return my_.quantity == 0 ? id : -1;
}


int Crypto::post_order (string const& userKey, Order const& order) {
    auto my = order;
    my.userId = user_verification (userKey);
    my.closed = "no";

    auto reversType = (my.type == "sell" ? "buy" : "sell");
    auto reversOrders = get_orders_where (
        "WHERE order.type = '{}' AND order.closed = 'no'", reversType
    );

    for (auto& other : reversOrders) {
        if (my.id == other.id)
            continue;

        if (is_profit (my, other)) {
            auto id = deal (my, other);

            if (id > 0) {
                return id;
            }
        }
    }

    return add_order (my);
}


void Crypto::delete_order (string const& userKey, int orderId) {
    auto userId = user_verification (userKey);
    auto order = check_and_get_user_order (userId, orderId);

    delete_order (order);
}


void Crypto::delete_order (Order const& order) {
    auto pair = get_pair (order.pairId);
    auto lotId = (order.type == "sell" ? pair.firstLotId : pair.secondLotId);
    auto price = order.quantity * (order.type == "sell" ? 1 : order.price);

    change_balance (order.userId, lotId, +price);
    db_request ("DELETE FROM order WHERE order.order_pk = '{}'", order.id);
}


vector<Crypto::Lot> Crypto::get_lots() {
    return get_lots_where ("");
}


Crypto::Pair Crypto::get_pair (int pairId) {
    auto str = db_request (
        "SELECT pair.pair_pk, pair.sale_lot_id, pair.buy_lot_id "
        "FROM pair "
        "WHERE pair.pair_pk = '{}'", pairId
    );

    return parse<Pair>(str).front();
}


vector<Crypto::Pair> Crypto::get_pairs() {
    auto str = db_request ("SELECT pair.pair_pk, pair.sale_lot_id, pair.buy_lot_id FROM pair");
    return parse<Pair> (str);
}


Crypto::Balance Crypto::get_balance (string const& userKey) {
    auto userId = user_verification (userKey);

    auto userLots = get_user_lots_where (
        "WHERE user_lot.user_id = '{}'", userId
    );

    Balance balance;

    for (auto& userLot : userLots) {
        balance[userLot.lotId] = userLot.quantity;
    }

    return balance;
}


Crypto::Crypto (string const& ip, size_t port, vector<string> lots)
    : client (ip, port)
{
    if (not is_init()) {
        init (lots);
    }
}


#include <iostream>
#include "db_client.cpp"
#include "checking_errors.cpp"
#include "../data_struct/string_view.cpp"

int main()
{
    vector<string> lots {
        "RUB",
        "BTC",
        "ETH",
        "USDT",
        "USDC"
    };

    try {
        Crypto crypto("0.0.0.0", 7432, lots);

        auto k1 = crypto.post_user ("Misha");
        auto k2 = crypto.post_user ("Sasha");
        cout << k1 << endl << k2 << endl;

        // auto k3 = crypto.post_user ("Kola4");
        // auto k4 = crypto.post_user ("Tola4");
        
        // auto id = crypto.find_user ("", k2);
        // cout << "id " << id << endl;
        // crypto.change_balance (id, 3, 999);

        // for (auto& [id, sell, buy] : crypto.get_pairs()) {
        //     cout << id << " " << sell << " " << buy << endl;
        // }

        // for (auto& [id, name] : crypto.get_lots()) {
        //     cout << id << " " << name << endl;
        // }

        Crypto::Order m{
            .pairId = 1,
            .quantity = 300,
            .price = 0.015,
            .type = "buy",
        };

        Crypto::Order s {
            .pairId = 1,
            .quantity = 200,
            .price = 0.01,
            .type = "sell",
        };

        crypto.post_order (k1, m);
        crypto.post_order (k2, s);

    } catch (exception const& e) {
        cout << "Ooops..\n" << e.what() << endl;
    }
}