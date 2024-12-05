#ifndef Pair_H_GUARD
#define Pair_H_GUARD

#include <string>
#include "lot.h"

struct Pair {
    int id;
    int firstLotId;
    int secondLotId;

public:
    static std::string add (Lot, Lot);
    static std::string get_all();
    static std::string search_by_id (int id);
    static Pair from_str (std::string const&);
};

#endif