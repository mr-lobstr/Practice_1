#include <fstream>
#include "iterator_by_rows.h"
#include "table.h"
#include "table_state.h"
#include "../data_struct/list.h"

using namespace std;
using namespace data_struct;


class IteratorByRows::IterImpl {
public:
    IterImpl (IterImpl const&) = delete;
    IterImpl (IterImpl&&) = delete;

    IterImpl (Table const& table_, TMode mode_)
        : table (const_cast<Table&> (table_))
        , stateGuard (table.state, mode_)
    {
        init();
    }

    ~IterImpl() noexcept {
        reset();
    };

    void init() {
        currentPage  = 1;
        readRowsCnt  = 0;
        pagesLoaded  = 0;
        rowsDeleted  = 0;

        pagesCnt = table.state.current_page();

        while (pagesLoaded < min(pagesCnt, maxPageCnt)) {
            next_page_load();
        }

        currRowIt = rows.begin();
    }

    void reset() {
        if (rowsDeleted == 0)
            return;

        while (not is_end()) {
            next();
        }

        if (not rows.empty()) {
            current_page_upload ();
        }

        while (currentPage <= pagesCnt) {
            table.fm.delete_page (currentPage++);
        }
    }

    void next() {
        ++currRowIt;
        ++readRowsCnt;

        if (one_page_read()) {
            current_page_upload();
            next_page_load();
        }
    }

    bool is_end() const noexcept {
        return currRowIt == rows.end();
    }

    string const& get_row() const noexcept {
        return *currRowIt;
    }

    StringView get_row_element (Column const& column) const {
        auto row = split (get_row(), ','); 
        return table.get_element_from (row, column);
    }

    void erase()
    {   
        if (rowsDeleted == 0) {
            table.state.set_position (currentPage - 1, table.rows_limit());
        }

        currRowIt = rows.erase (currRowIt);
        ++rowsDeleted;

        if (one_page_deleted()) {
            next_page_load();
        }
    }

    TMode mode() const noexcept {
        return stateGuard.mode();
    }

private:
    bool one_page_read() const noexcept {
        return readRowsCnt != 0
           and readRowsCnt % table.rows_limit() == 0;
    }

    bool one_page_deleted() const noexcept {
        return rowsDeleted != 0 
           and rowsDeleted % table.rows_limit() == 0;
    }

    void next_page_load() {
        auto loadingPage = pagesLoaded + 1;

        if (loadingPage > pagesCnt)
            return;

        try {
            page_load (loadingPage);
        } catch (runtime_error& re) {
            throw runtime_error (
                "ошибка при чтении страницы " + to_string (loadingPage) + ":\n" + re.what()
            );
        }

        ++pagesLoaded;
    }

    void page_load (PageNumb loadingPage) {
        table.fm.read_page (loadingPage, [&] (ifstream& fPage) {
            string line;
            getline (fPage, line);

            while (getline (fPage, line)) {
                rows.push_back (std::move (line));
            }
        });
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
        table.fm.write_page (currentPage, [&] (ofstream& fPage) {
            size_t i = 1;

            for (auto beg = rows.begin(); beg != currRowIt; ++beg) {
                fPage << *beg << "\n";
                table.state.set_position (currentPage, i++);
            }
        });
    }

private:
    Table& table;
    TStateGuard stateGuard;

    Rows rows{};
    ListIter currRowIt{};

    std::size_t currentPage  = 1;
    std::size_t readRowsCnt  = 0;
    std::size_t pagesLoaded  = 0;
    std::size_t rowsDeleted  = 0;
    std::size_t pagesCnt     = 0;

    static constexpr std::size_t maxPageCnt = 50;
};