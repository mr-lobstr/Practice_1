#ifndef USER_LOT_H_GUARD
#define USER_LOT_H_GUARD

#include <string>

struct UserLot {
    int id;
    int userId;
    int lotId;
    double quantity;

public:
    static std::string add (int userId, int lotId, double quantity);
    static std::string del (UserLot const&);
    static std::string get_all();
    static std::string search_by_user_id (int userId);
    static std::string search_one (int userId, int lotId);

    static UserLot from_str (std::string const&);
};

#endif