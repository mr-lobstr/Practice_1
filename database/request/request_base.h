#ifndef REQUEST_BASE_H_GUARD
#define REQUEST_BASE_H_GUARD

#include <memory>


class Request {
public:
    enum class Type {
        AddTable
      , Insert
      , Delete
      , Select
    };

public:
    Request (Type type_)
        : type (type_)
    {}

    virtual ~Request() = default;

public:
    Type type;
};


using RequestPtr = std::unique_ptr<Request>;

#endif