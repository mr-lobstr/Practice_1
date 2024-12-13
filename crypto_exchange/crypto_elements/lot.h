#ifndef LOT_H_GUARD
#define LOT_H_GUARD

#include <string>
#include <nlohmann/json.hpp>

struct Lot {
    int id;
    std::string name;

public:
    static std::string add (std::string const& name);
    static std::string get_all();
    static std::string search_by_id (int id);
    static Lot from_str (std::string const&);
};

void to_json (nlohmann::json&, Lot const&);

#endif