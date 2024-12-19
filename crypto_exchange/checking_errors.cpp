#include <format>
#include "crypto.h"
using namespace std;


void check_input (string const& str) {
    auto it = find_if (str.begin(), str.end(), [] (char c) {
        return c == '\n'
            or c == '\''
            or c == ',';
    });

    if (it != str.end()) throw invalid_argument (
        format ("недопустимый символ: {}\n", *it)
    );
}


int Crypto::user_verification (string const& key) {
    check_input (key);

    auto user = get_one<User> (User::search_by_key (key));
    
    if (user.id == 0) throw runtime_error (
        format ("пользователя с ключом {} не существует\n", key)
    );

    return user.id;
}


void Crypto::check_repeat_username (string const& name) {
    check_input (name);
    auto users = get_parse<User> (User::search_by_name (name));

    if (users.size() != 0) throw invalid_argument (
        format ("пользователь с именем {} уже существует\n", name)
    );
}


void Crypto::check_change_quantity (double sum, UserLot const& userLot) {
    if (sum > 0 or abs (sum) <= userLot.quantity)
        return;

    auto lot = get_one<Lot> (Lot::search_by_id (userLot.lotId));

    throw invalid_argument ( format (
        "у пользователя на счету {} {}, невозможно снять {} {}",
        userLot.quantity, lot.name, sum, lot.name
    ));
}


Order Crypto::check_and_get_user_order (int userId, int orderId) {
    auto request = Order::search_one (userId, orderId);

    try {
        return get_one<Order> (request);
    } catch (invalid_argument const& e) {
        throw invalid_argument (format (
            "пользователь не выставлял ордер с id = '{}'", orderId
        ));
    }
}