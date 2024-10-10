#ifndef ITERATOR_BY_PAGES_H_GUARD
#define ITERATOR_BY_PAGES_H_GUARD

#include <string>
#include <fstream>
#include "file_manager.h"
#include "data_struct/dynamic_array.h"
#include "data_struct/list.h"

class IterByPages {
    using Lines = data_struct::List<
        data_struct::DynamicArray<std::string>
    >;

    using IterByLines = typename Lines::const_iterator;

public:
    IterByPages (Table const&, TableFileManager const&);
    ~IterByPages();

    IterByPages (IterByPages const&) = delete;
    IterByPages (IterByPages&&)      = delete;

    IterByPages& operator= (IterByPages const&) = delete;
    IterByPages& operator= (IterByPages&&)      = delete;

    void next();
    void erase();
    bool is_end() const noexcept;
    IterByLines current_line_it() const noexcept;

private:
    void page_load();
    void page_upload (IterByLines, IterByLines);
    void page_write_lines (std::size_t, IterByLines, IterByLines);

private:
    Table const& table;
    TableFileManager const& fm; 

    Lines lines;
    IterByLines currLineIt;

    std::size_t currPage     = 1;
    std::size_t currLine     = 1;
    std::size_t lastLoadPage = 0;
    std::size_t cntDelLines  = 0;

    std::size_t lastPage   = 0;

    static constexpr std::size_t maxPageCnt = 50;
};


#endif