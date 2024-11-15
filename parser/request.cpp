#include "request.h"
using namespace std;

Request::Request (string str_)
    : str (move (str_))
{}


Request::Request (Request&& rhs) noexcept
    : str (move (rhs.str))
{}


InsertRequest::InsertRequest (string str_)
    : Request (move (str_))
{}


string InsertRequest::execute (Database& db) {
    return db.insert (tableName, row);
}


DeleteRequest::DeleteRequest (string str_)
    : Request (move (str_))
{}


string DeleteRequest::execute (Database& db) {
    return db.erase (tableName, condition);
}


SelectRequest::SelectRequest (string str_)
    : Request (move (str_))
{}


string SelectRequest::execute (Database& db) {
    return db.select (tablesNames, tcPairs);
}


FilterRequest::FilterRequest (SelectRequest select)
    : SelectRequest (move (select))
{}


string FilterRequest::execute (Database& db) {
    return db.filter (tablesNames, tcPairs, condition);
}