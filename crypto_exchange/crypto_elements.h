#ifndef CRYPTO_ELEMENTS_H_GUARD
#define CRYPTO_ELEMENTS_H_GUARD

#include <map>
#include <vector>
#include <string>

#include "./crypto_elements/state.h"
#include "./crypto_elements/lot.h"
#include "./crypto_elements/pair.h"
#include "./crypto_elements/user.h"
#include "./crypto_elements/user_lot.h"
#include "./crypto_elements/order.h"


std::vector<std::string> split (std::string const&, char);


template <typename T>
std::vector<T> parse (std::string const& str) {
    std::vector<T> arr;

    if (str == "")
        return {};

    for (auto& line : split (str, '\n')) {
        arr.push_back (T::from_str (line));
    }
    
    return arr;
}


struct Balance {
    std::map<int, double> data;
};

#endif