#ifndef USER_H_GUARD
#define USER_H_GUARD

#include <string>

struct User {
    int id;
    std::string name;
    std::string key;

public:
    static std::string add (std::string const&, std::string const&);
    static std::string key_gen();
    static std::string get_all();
    static std::string search_by_id (int id);
    static std::string search_by_name (std::string const&);
    static std::string search_by_key (std::string const&);

    static User from_str (std::string const&);
};

#endif