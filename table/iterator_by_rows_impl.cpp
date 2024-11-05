#include <fstream>
#include "../data_struct/list.h"
#include "iterator_by_rows.h"
#include "table.h"
#include "table_state_guard.h"

using namespace std;
using namespace data_struct;


using RowsList = data_struct::List<string>;
using ListIter = typename RowsList::const_iterator;


class IteratorByRows::IterImpl {
public:
    IterImpl (Table const& table_)
        : table (table_)
        , stateGuard (table_)
        , pagesCnt (stateGuard.current_page())
    {
        auto minNumb = min(pagesCnt, maxPageCnt);
        while (pagesLoaded < minNumb) {
            next_page_load();
        }

        currRowIt = rows.begin();
    }

    ~IterImpl() noexcept {
        while (rowsDeleted != 0 and not is_end()) {
            next();
        }

        if (not rows.empty()) {
            current_page_upload ();
        }

        if (rowsDeleted > 0) {
            while (currentPage <= pagesCnt) {
                table.fm.delete_page(currentPage++);
            }
        }
        };

    IterImpl (IterImpl const&) = delete;
    IterImpl (IterImpl&&) = delete;

    void next() {
        ++currRowIt;
        if (readRowsCnt % table.rows_limit() == 0) {
            current_page_upload();
            next_page_load();
        }
        ++readRowsCnt;
    }

    string const& get_row() const noexcept {
        return *currRowIt;
    }

    StringView get_row_element (string const& column) const {
        size_t ind = (column == table.name + "_pk")
                   ? 0
                   : table.columns [column];

        auto views = split (get_row(), ','); 
        return views[ind];
    }

    void erase()
    {   
        if (rowsDeleted == 0) {
            stateGuard.set_position (currentPage - 1, table.rows_limit());
        }

        currRowIt = rows.erase (currRowIt);
        ++rowsDeleted;

        if (rowsDeleted % table.rows_limit() == 0) {
            next_page_load();
        }
    }

    bool is_end() const noexcept {
        return currRowIt == rows.end();
    }
private:
    void next_page_load()
    {
        auto loadingPage = pagesLoaded + 1;

        if (loadingPage > pagesCnt)
            return;

        table.fm.read_page (loadingPage, [&] (ifstream& fPage) {
            string line;
            getline (fPage, line);

            while (getline (fPage, line)) {
                rows.push_back (std::move (line));
            }
        });

        ++pagesLoaded;
    }

    void current_page_upload() {
        if (currentPage > pagesCnt)
            return;

        if (rowsDeleted > 0) {
            page_write();
        }

        rows.erase (rows.begin(), currRowIt);
        ++currentPage;
    }

    void page_write() {
        size_t cnt = 0;

        table.fm.write_page (currentPage, [&] (ofstream& fPage) {
            auto beg = rows.begin();

            while (beg != currRowIt) {
                fPage << *beg << "\n";
                ++beg;
                ++cnt;
            }
        });

        stateGuard.set_position (currentPage, cnt);
    }

private:
    Table const& table;
    TableStateGuard stateGuard;

    RowsList rows{};
    ListIter currRowIt{};

    std::size_t currentPage  = 1;
    std::size_t readRowsCnt   = 1;
    std::size_t pagesLoaded  = 0;
    std::size_t rowsDeleted  = 0;
    std::size_t pagesCnt     = 0;

    static constexpr std::size_t maxPageCnt = 50;
};