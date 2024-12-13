#include <format>
#include <vector>
#include "lot.h"
using namespace std;
using namespace nlohmann;

vector<string> split (string const&, char);

Lot Lot::from_str (string const& str) {
    auto values = split (str, ',');

    if (str == "")
        return {};
    
    return {
        stoi (values[0])
      , values[1]
    };
}


string Lot::add (string const& name) {
    return format ("INSERT INTO lot VALUES ('{}')", name);
}


string Lot::get_all() {
    return "SELECT lot.lot_pk, lot.name FROM lot ";
}


string Lot::search_by_id (int id) {
    return get_all() 
         + format ("WHERE lot.lot_pk = '{}'", id);
}


void to_json (json& j, Lot const& lot) {
    j["lot_id"] = lot.id;
    j["name"] = lot.name;
}