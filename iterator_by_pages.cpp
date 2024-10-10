#include "iterator_by_pages.h"
#include "table.h"

using namespace std;
using namespace filesystem;
using namespace data_struct;


auto split (string const& str, char separator) {
    DynamicArray<string> res;
    auto subStrBeg = str.begin();

    while (subStrBeg != str.end()) {
        if (subStrBeg != str.begin()) {
            ++subStrBeg;
        }
        auto subStrEnd = algs::find (subStrBeg, str.end(), separator);
        res.push_back (string (subStrBeg, subStrEnd));
        subStrBeg = subStrEnd;
    }

    return res;
}


IterByPages::IterByPages (Table const& table_, TableFileManager const& fm_)
    : table (table_)
    , fm (fm_)
    , lines ()
    , lastPage (fm.get_position().first)
{
    if (fm.locked()) {
        // sleep
    }

    fm.lock();
    auto minNumb = min(lastPage, maxPageCnt);
    while (lastLoadPage < minNumb) {
        page_load();
    }

    currLineIt = lines.begin();
}


IterByPages::~IterByPages() {
    size_t lastLine = currLine % table.linesLimit
                    ? currLine % table.linesLimit
                    : table.linesLimit;

    if (not lines.empty()) {
        page_upload (lines.begin(), lines.end());
        ++currPage;
    }

    if (cntDelLines > 0) {
        fm.set_position (currPage - 1, lastLine);

        while (currPage <= lastPage) {
            fm.delete_page(currPage++);
        }
    }

    fm.unlock();
    lines.~List();
}


void IterByPages::page_load()
{
    auto loadingPage = lastLoadPage + 1;

    if (loadingPage > lastPage)
        return;

    ifstream file (fm.page_file_name (loadingPage));
    string line;

    getline (file, line);
    while (getline (file, line)) {
        lines.push_back (split (line, ','));
    }
    file.close();

    ++lastLoadPage;
}


void IterByPages::page_upload (IterByLines beg, IterByLines end) {
    if (currPage > lastPage)
        return;

    if (cntDelLines > 0) {
        fm.creat_page (currPage);
        page_write_lines (currPage, beg, end);
    }

    lines.erase (beg, end);
}


void IterByPages::next() {
    ++currLineIt;
    if (currLine % table.linesLimit == 0) {
        page_upload (lines.begin(), currLineIt);
        page_load();
        ++currPage;
    }
    ++currLine;
}


void IterByPages::erase()
{
    currLineIt = lines.erase (currLineIt);
    ++cntDelLines;

    if (cntDelLines % table.linesLimit == 0) {
        page_load();
    }
}


bool IterByPages::is_end() const noexcept {
    return currLineIt == lines.end();
}


IterByPages::IterByLines IterByPages::current_line_it() const noexcept {
    return currLineIt;
}


void IterByPages::page_write_lines(size_t numb, IterByLines beg, IterByLines end) {
    ofstream file (fm.page_file_name (numb), ios::app);
    while (beg != end) {
        fm.page_write_line (file, *beg++);
    }
    file.close();
}