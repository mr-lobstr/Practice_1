#include <format>
#include <vector>
#include "order.h"
using namespace std;
using namespace nlohmann;

vector<string> split (string const&, char);

Order Order::from_str (string const& str) {
    auto values = split (str, ',');

    if (str == "")
        return {};

    return {
        stoi (values[0])
      , stoi (values[1])
      , stoi (values[2])
      , stod (values[3])
      , stod (values[4])
      , values[5]
      , values[6]
    };
}



string Order::add (Order const& order) {
    return format (
        "INSERT INTO order "
        "VALUES ('{}', '{}', '{}', '{}', '{}', '{}')",
        order.userId
      , order.pairId
      , order.quantity
      , order.price
      , order.type
      , order.closed
    );
}


string Order::del (Order const& order) {
    return format (
        "DELETE FROM order WHERE order.order_pk = '{}'",
        order.id
    );
}


string Order::get_all() {
    return
    "SELECT  order.order_pk, "
            "order.user_id, "
            "order.pair_id, "
            "order.quantity, "
            "order.price, "
            "order.type, "
            "order.closed "
    "FROM order ";
}


string Order::search_by_user_id (int userId) {
    return get_all()
         + format ("WHERE order.user_id = '{}'", userId);
}


string Order::search_one (int userId, int orderId) {
    return get_all()
         + format (
            "WHERE order.order_pk = '{}' AND order.user_id = '{}'",
            userId, orderId
        );
}


string Order::search_revers (Order const& order) {
    auto reversType = (order.type == "sell" ? "buy" : "sell");

    return get_all()
         + format (
            "WHERE order.pair_id = '{}' AND order.type = '{}' AND order.closed = 'no'",
            order.pairId, reversType
        );
}


void to_json (json& j, Order const& order) {
    j["order_id"] = order.id;
    j["user_id"] = order.userId;
    j["pair_id"] = order.pairId;
    j["quantity"] = order.quantity;
    j["type"] = order.type;
    j["price"] = order.price;
    j["closed"] = order.closed;
}