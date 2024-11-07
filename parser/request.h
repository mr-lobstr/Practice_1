#ifndef REQUEST_H_GUARD
#define REQUEST_H_GUARD

#include <string>
#include "../database/database.h"


class Request {
public:
    Request (std::string);
    Request (Request&&) noexcept;

    virtual void execute (Database&) = 0;
    virtual ~Request() = default;

protected:
    std::string str{};
};


class InsertRequest: public Request {
    friend class Parser;
public:
    InsertRequest (std::string);
    void execute (Database& db) override;

private:
    std::string tableName{};
    Table::Row row{};
};


class DeleteRequest: public Request {
    friend class Parser;
public:
    DeleteRequest (std::string);
    void execute (Database& db) override;

private:
    std::string tableName{};
    Condition condition{};
};


class SelectRequest: public Request {
    friend class Parser;
public:
    SelectRequest (std::string);
    void execute (Database& db) override;

protected:
    TablesNames tablesNames{};
    TableColumnPairs tcPairs{};
};


class FilterRequest: public SelectRequest {
    friend class Parser;
public:
    FilterRequest (SelectRequest select);
    void execute (Database& db) override;

private:
    Condition condition{};
};

#endif