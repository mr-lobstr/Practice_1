#ifndef CRYPTO_EXCHANGE_H_GUARD
#define CRYPTO_EXCHANGE_H_GUARD

#include "crypto.h"
#include "../data_struct/string_view.h"


struct Crypto::Lot {
    int id;
    std::string name;
};


struct Crypto::Pair {
    int id;
    int firstLotId;
    int secondLotId;
};


struct Crypto::User {
    int id;
    std::string name;
    std::string key;
};


struct Crypto::UserLot {
    int id;
    int userId;
    int lotId;
    double quantity;
};


struct Crypto::Order {
    int id;
    int userId;
    int pairId;

    double quantity;
    double price;

    std::string type;
    std::string closed;
};


template <typename T>
T from_str (std::string const&);


template <>
Crypto::Lot from_str<Crypto::Lot> (std::string const& str) {
    auto values = split (str, ',');
    using namespace std;
    
    return {
        stoi (values[0])
      , values[1]
    };
}


template <>
Crypto::Pair from_str<Crypto::Pair> (std::string const& str) {
    auto values = split (str, ',');
    using namespace std;

    return {
        stoi (values[0])
      , stoi (values[1])
      , stoi (values[2])
    };
}


template <>
Crypto::User from_str<Crypto::User> (std::string const& str) {
    auto values = split (str, ',');
    using namespace std;

    if (str == "")
        return {};

    return {
        stoi (values[0])
      , values[1]
      , values[2]
    };
}


template <>
Crypto::UserLot from_str<Crypto::UserLot> (std::string const& str) {
    auto values = split (str, ',');
    using namespace std;

    if (str == "")
        return {};

    return {
        stoi (values[0])
      , stoi (values[1])
      , stoi (values[2])
      , stod (values[3])
    };
}


template <>
Crypto::Order from_str<Crypto::Order> (std::string const& str) {
    auto values = split (str, ',');
    using namespace std;

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


template <typename T>
std::vector<T> parse (std::string const& str) {
    std::vector<T> arr;

    if (str == "")
        return {};

    for (auto& line : split (str, '\n')) {
        arr.push_back (from_str<T> (line));
    }
    
    return arr;
}

#endif