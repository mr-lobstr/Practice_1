#include <iostream>
#include <sstream>
#include <chrono>
#include <format>
#include "crypto.h"
using namespace std;
using namespace chrono;

string Crypto::db_request (string const& request) {
    auto [success, responce] = client.send_request (request);

    if (not success) throw runtime_error (
        responce
    );

    return responce;
}


bool Crypto::is_init() {
    return "init" == db_request (State::is_init());;
}


void Crypto::init (vector<string> const& lotsNames) {
    db_request (State::init (1000));

    for (auto& name : lotsNames) {
        add<Lot> (name);
    }

    auto lots = get_all<Lot>();

    for (size_t i = 0; i < lots.size(); ++i) {
        for (size_t j = i + 1; j < lots.size(); ++j) {
            add<Pair> (lots[i], lots[j]);
        }
    }
}


string Crypto::post_user (string const& name) {
    check_repeat_username (name);
    auto key = User::key_gen();
    auto userId = add<User> (name, key);
    auto quantity = stod (db_request (State::get_default_quantity()));

    for (auto& [lotId, _] : get_all<Lot>()) {
        add<UserLot> (userId, lotId, quantity);
    }

    return key;
}


bool is_profit (Order const& my, Order const& other) {
    return my.type == "sell"
         ? my.price <= other.price
         : my.price >= other.price;
}


void Crypto::change_balance (int userId, int lotId, double sum) {
    auto userLot = get_one<UserLot> (UserLot::search_one(userId, lotId));

    check_change_quantity (sum, userLot);
    userLot.quantity += sum;

    db_request (UserLot::del (userLot));
    add<UserLot> (userId, lotId, userLot.quantity);
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


int Crypto::create_order (Order const& order) {
    auto id = add<Order> (order);

    auto [_, lot1, lot2] = get_one<Pair> (Pair::search_by_id(order.pairId));
    auto lot = (order.type == "sell" ? lot1 : lot2);
    auto sum = (order.type == "sell" ? 1 : order.price) * order.quantity;
    
    change_balance (order.userId, lot, -sum);

    return id;
}


int Crypto::closed_order (Order const& order) {
    auto [_, lot1, lot2] = get_one<Pair> (Pair::search_by_id(order.pairId));
    auto lot = (order.type == "sell" ? lot2 : lot1);
    auto sum = (order.type == "sell" ? order.price : 1) * order.quantity;

    change_balance (order.userId, lot, +sum);

    return create_order (order);
}

Order order_separate (Order& order, double quantity) {
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
        create_order (other_);
    }

    return my_.quantity == 0 ? id : -1;
}


int Crypto::post_order (string const& userKey, Order const& order) {
    auto my = order;
    my.userId = user_verification (userKey);
    my.closed = "no";

    auto reversOrders = get_parse<Order> (Order::search_revers (my));

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

    return create_order (my);
}


void Crypto::delete_order (string const& userKey, int orderId) {
    auto userId = user_verification (userKey);
    auto order = check_and_get_user_order (userId, orderId);

    delete_order (order);
}


void Crypto::delete_order (Order const& order) {
    auto pair = get_one<Pair> (Pair::search_by_id(order.pairId));
    auto lotId = (order.type == "sell" ? pair.firstLotId : pair.secondLotId);
    auto price = order.quantity * (order.type == "sell" ? 1 : order.price);

    change_balance (order.userId, lotId, +price);
    db_request (Order::del (order));
}


Crypto::Balance Crypto::get_balance (string const& userKey) {
    auto userId = user_verification (userKey);
    auto userLots = get_parse<UserLot> (UserLot::search_by_user_id (userId));

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
#include "crypto_elements/lot.cpp"
#include "crypto_elements/pair.cpp"
#include "crypto_elements/user.cpp"
#include "crypto_elements/user_lot.cpp"
#include "crypto_elements/order.cpp"
#include "crypto_elements/split.cpp"

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

        auto k1 = crypto.post_user ("Misha2");
        auto k2 = crypto.post_user ("Sasha2");
        cout << k1 << endl << k2 << endl;

        Order m{
            .pairId = 1,
            .quantity = 300,
            .price = 0.015,
            .type = "buy",
        };

        Order s {
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