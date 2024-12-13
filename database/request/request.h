#ifndef REQUEST_H_GUARD
#define REQUEST_H_GUARD

#include "request_base.h"
#include "../database/database_fwd.h"
#include "../table/table.h"
#include "../data_struct/dynamic_array.h"
#include "../data_struct/binary_tree.h"


class AddRequest: public Request {
    friend class Parser;
    friend Database; 

public:
    AddRequest()
        : Request (Type::AddTable)
    {}

private:
    TableName tableName{};
    Columns columns{};
};


class InsertRequest: public Request {
    friend class Parser;
    friend Database; 

public:
    InsertRequest()
        : Request (Type::Insert)
    {}

private:
    std::string tableName{};
    Table::Row row{};
};


class DeleteRequest: public Request {
    friend class Parser;
    friend Database;

public:
    DeleteRequest()
        : Request (Type::Delete)
    {}

private:
    std::string tableName{};
    Condition condition{};
};


class SelectRequest: public Request {
    friend class Parser;
    friend Database;

public:
    SelectRequest()
        : Request (Type::Select)
    {}

private:
    TableName outTable{};
    TablesNames tablesNames{};
    TableColumnPairs tcPairs{};
    Condition condition{};
};

#endif