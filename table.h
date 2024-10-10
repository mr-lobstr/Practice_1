#ifndef TABLE_H_GUARD
#define TABLE_H_GUARD

#include "file_manager.h"
#include "iterator_by_pages.h"

#include "data_struct/dynamic_array.h"
#include "data_struct/hash_table.h"
#include "data_struct/hash_set.h"


class Table {
    using Array = data_struct::DynamicArray<std::string>;

    friend TableFileManager;
    friend IterByPages;
    struct IterImpl;

public:
    using Iterator = data_struct::ForwardIterator<Array, IterImpl, data_struct::Const_tag>;

public:
    Table (std::string const&);
    ~Table() noexcept;

    Table& set_path (std::string const&);
    Table& set_limit (std::size_t);

    void create_files() const;
    
    template <typename Iter>
    Table& set_columns (Iter beg, Iter end) {
        columns.push_back (name + "_pk");
        algs::copy (beg, end, algs::back_inserter (columns));
        
        return *this;
    }

    Iterator begin() const;
    Iterator end() const;

    std::string const& get_element (Iterator, std::string const&) const;

    void erase (Iterator);
    void insert_back (Array const&);

private:
    void create_iter() const;
    void delete_iter() const;

private:
    TableFileManager fm;

    std::string name{};
    Array columns{};
    std::size_t linesLimit = 0;

    mutable IterByPages* ptrIter = nullptr;
};


struct Table::IterImpl {
    using Container = Table;

public:
    IterByPages* real() const noexcept {
        return ptr->ptrIter;
    }

    bool equal (IterImpl const& rhs) const {
        if (rhs.ptr == nullptr) {
            if (real()->is_end()) {
                ptr->delete_iter();
                return true;
            }
            return false;
        }
        return ptr == rhs.ptr;
    }

    void next() const {
        real()->next();
    }

    auto& get_value() const noexcept {
        return *real()->current_line_it();
    }

public:
    Table* ptr = nullptr;
};

#endif