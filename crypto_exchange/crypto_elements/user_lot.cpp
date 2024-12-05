#include <format>
#include <vector>
#include "user_lot.h"
using namespace std;

vector<string> split (string const&, char);

UserLot UserLot::from_str (string const& str) {
    auto values = split (str, ',');

    if (str == "")
        return {};

    return {
        stoi (values[0])
      , stoi (values[1])
      , stoi (values[2])
      , stod (values[3])
    };
}


string UserLot::del (UserLot const& userLot) {
    return format (
        "DELETE FROM user_lot WHERE user_lot.user_lot_pk = '{}'",
        userLot.id
    );
}


string UserLot::add (int userId, int lotId, double quantity) {
    return format (
        "INSERT INTO user_lot VALUES ('{}', '{}', '{}')",
        userId, lotId, quantity
    );
}


string UserLot::get_all() {
    return
    "SELECT  user_lot.user_lot_pk, "
            "user_lot.user_id, "
            "user_lot.lot_id, "
            "user_lot.quantity "
    "FROM user_lot ";
}


string UserLot::search_by_user_id (int userId) {
    return get_all()
         + format ("WHERE user_lot.user_id = '{}'", userId);
}


string UserLot::search_one (int userId, int lotId) {
    return get_all()
         + format (
            "WHERE user_lot.user_id = '{}' AND user_lot.lot_id = '{}'"
            , userId, lotId
         );
}