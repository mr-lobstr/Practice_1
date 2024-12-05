#include <format>
#include <vector>
#include "pair.h"
using namespace std;

vector<string> split (string const&, char);

Pair Pair::from_str (string const& str) {
    auto values = split (str, ',');

    if (str == "")
        return {};
    
    return {
        stoi (values[0])
      , stoi (values[1])
      , stoi (values[2])
    };
}


string Pair::add (Lot lot1, Lot lot2) {
    return format (
        "INSERT INTO pair VALUES ('{}', '{}')", lot1.id, lot2.id
    );
}


string Pair::get_all() {
    return "SELECT pair.pair_pk, "
                  "pair.sale_lot_id, "
                  "pair.buy_lot_id "
            "FROM pair ";
}


string Pair::search_by_id (int id) {
    return get_all()
         + format ("WHERE pair.pair_pk = '{}'", id);
}