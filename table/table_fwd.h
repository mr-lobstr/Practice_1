#ifndef TABLE_FORWARD_H_GUARD
#define TABLE_FORWARD_H_GUARD

#include <cstdint>
#include <string>
#include "../data_struct/data_struct_fwd.h"


class Database;
class Table;
class IteratorByRows;
class TState;
class TFileManager;


enum class TMode {
    free
  , reading
  , writing
};


using PrimeKey = std::size_t;

using RowNumb = std::size_t;
using PageNumb = std::size_t;
using ColumnNumb = std::size_t;

using Column = std::string;
using TableName = std::string;

using TablesNames = ds::DynamicArray<TableName>;
using TableColumn = ds::Pair<StringView, StringView>;
using TableColumnPairs = ds::DynamicArray<TableColumn>;

using Condition = ds::BinTree<std::string>;

#endif