#include <format>
#include <random>
#include <chrono>
#include "user.h"
using namespace std;
using namespace chrono;

vector<string> split (string const&, char);

User User::from_str (string const& str) {
    auto values = split (str, ',');
    
    if (str == "")
        return {};

    return {
        stoi (values[0])
      , values[1]
      , values[2]
    };
}


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


string User::add (string const& name, string const& key) {
    return format (
        "INSERT INTO user VALUES ('{}', '{}')", name, key
    );
}

string User::key_gen() {
    return random_string (50);
}


string User::get_all() {
    return "SELECT user.user_pk, user.name, user.key FROM user ";
}


string User::search_by_id (int id) {
    return get_all()
         + format ("WHERE user.user_pk = '{}'", id);
}


string User::search_by_name (std::string const& name) {
    return get_all()
         + format ("WHERE user.name = '{}'", name);
}


string User::search_by_key (std::string const& key) {
    return get_all()
         + format ("WHERE user.key = '{}'", key);
}