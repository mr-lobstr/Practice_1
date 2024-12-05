#include <format>
#include "crypto_exchange.h"
using namespace std;


int Crypto::user_verification (string const& userKey) {
    auto user = get_user_where (
        "WHERE user.key = '{}'", userKey
    );
    
    if (user.id == 0) throw runtime_error (
        format ("пользователя с ключом {} не существует", userKey)
    );

    return user.id;
}


void Crypto::check_repeat_username (string const& name) {
    auto user = get_user_where (
        "WHERE user.name = '{}'", name
    );

    if (user.id != 0) throw invalid_argument (
        format ("пользователь с именем {} уже существует\n", name)
    );
}


void Crypto::check_change_quantity (double sum, UserLot const& userLot) {
    if (sum <= userLot.quantity)
        return;

    auto lot = get_lots_where (
        "WHERE lot.lot_pk = '{}'", userLot.lotId
    ).front();

    throw invalid_argument ( format (
        "у пользователя на счету {} {}, невозможно снять {} {}",
        userLot.quantity, lot.name, sum, lot.name
    ));
}


Crypto::Order Crypto::check_and_get_user_order (int userId, int orderId) {
    auto orders = get_orders_where (
        "WHERE order.order_pk = '{}' AND order.user_id = '{}'"
        , userId, orderId
    );

    if (not orders.empty()) {
        return orders.front();
    }
    
    throw invalid_argument (format (
        "пользователь не выставлял ордер с id = '{}'", orderId
    ));
}