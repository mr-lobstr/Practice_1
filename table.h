#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "file_manager.h"
#include "iterator_by_rows.h"
#include "data_struct/hash_table.h"


namespace data_struct
{
    template<>
    struct Hasher<std::string> {
        size_t operator() (std::string const& s) const {
            size_t hash = 2736712;

            for (auto c : s) {
                hash += 37 * c;
            }   
            return hash;
        }
    };
}


class Table {
    friend class TableStateGuard;

    using Columns = data_struct::HashTable<std::string, std::size_t>;

    friend TableFileManager;
    friend IteratorByRows;
    // struct IterImpl;

public:
    using Iterator = IteratorByRows;

    enum Lock {
        lock = true
      , unlock = false
    };

public:
    Table (std::string const&);
    ~Table() noexcept = default;

    Table& set_path (std::string const&);
    Table& set_limit (std::size_t);
    
    template <typename Iter>
    Table& set_columns (Iter beg, Iter end) {
        size_t index = 1;

        algs::for_each (beg, end, [&] (auto& column) {
            columns.add (column, index++);
        });
        
        return *this;
    }

    void create_files() const;

    Iterator begin() const;
    Iterator end() const;

    std::string get_element (Iterator&, std::string const&) const;

    void insert_back (std::string const&);
    void erase (Iterator&);

private:
    std::string header() const;

private:
    TableFileManager fm;

    std::string name{};
    std::string pathDir{};
    std::size_t rowsLimit = 0;

    Columns columns{};
};


// struct Table::IterImpl {
//     using Container = Table;
//
// public:
//     IterByPages* real() const noexcept {
//         return ptr->ptrIter;
//     }
//
//     bool equal (IterImpl const& rhs) const {
//         if (rhs.ptr == nullptr) {
//             if (real()->is_end()) {
//                 ptr->delete_iter();
//                 return true;
//             }
//             return false;
//         }
//         return ptr == rhs.ptr;
//     }
//
//     void next() const {
//         real()->next();
//     }
//
//     auto& get_value() const noexcept {
//         return *real()->current_line_it();
//     }
//
// public:
//     Table* ptr = nullptr;
// };

#endif