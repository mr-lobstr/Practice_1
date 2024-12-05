#ifndef ORDER_H_GUARD
#define ORDER_H_GUARD

#include <string>

struct Order {
    int id;
    int userId;
    int pairId;

    double quantity;
    double price;

    std::string type;
    std::string closed;

public:
    static std::string add (Order const&);
    static std::string del (Order const&);
    static std::string get_all();
    static std::string search_by_id (int id);
    static std::string search_by_user_id (int userId);
    static std::string search_one (int userId, int orderId);
    static std::string search_revers (Order const&);

    static Order from_str (std::string const&);
};

#endif