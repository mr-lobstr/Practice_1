#include <string>
#include <nlohmann/json.hpp>
#include "../crypto_exchange/crypto.h"

using namespace std;
using json = nlohmann::json;


json post_user (Crypto& crypto, json const& args) {
    auto userKey = crypto.post_user (args["username"]);
    return {{"key", userKey}};
}


json post_order (Crypto& crypto, json const& args, string const& key) {
    auto orderId = crypto.post_order (
        key,
        {
            .pairId = args["pair_id"].get<int>(),
            .quantity = args["quantity"].get<double>(),
            .price = args["price"].get<double>(),
            .type = args["type"]
        }
    );

    return {{"order_id", orderId}};
}


json get_order (Crypto& crypto) {
    return crypto.get<Order>();
}


json get_lot (Crypto& crypto) {
    return crypto.get<Lot>();
}


json get_pair (Crypto& crypto) {
    return crypto.get<Pair>();
}


json get_balance (Crypto& crypto, string const& key) {
    return crypto.get_balance (key);
}


json delete_order (Crypto& crypto, json const& args, string const& key) {
    crypto.delete_order (key, args["order_id"].get<int>());
    return {};
}