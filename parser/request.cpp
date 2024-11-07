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


void InsertRequest::execute (Database& db) {
    db.insert (tableName, row);
}


DeleteRequest::DeleteRequest (string str_)
    : Request (move (str_))
{}


void DeleteRequest::execute (Database& db) {
    db.erase (tableName, condition);
}


SelectRequest::SelectRequest (string str_)
    : Request (move (str_))
{}


void SelectRequest::execute (Database& db) {
    db.select (tablesNames, tcPairs);
}


FilterRequest::FilterRequest (SelectRequest select)
    : SelectRequest (move (select))
{}


void FilterRequest::execute (Database& db) {
    db.filter (tablesNames, tcPairs, condition);
}