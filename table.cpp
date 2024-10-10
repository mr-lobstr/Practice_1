#include "table.h"

using namespace std;
using namespace filesystem;
using namespace data_struct;

Table::Table (string const& name_)
    : name (name_)
    , fm (*this)
{}

Table::~Table() noexcept {
    if (ptrIter) {
        delete_iter();
    }
}

Table& Table::set_path (std::string const& path_) {
    fm.set_path_directory (path_);
    return *this;
}


Table& Table::set_limit (std::size_t limit) {
    linesLimit = limit;
    return *this;
}


void Table::create_files() const {
    fm.create_files();
}


void Table::create_iter() const {
    if (ptrIter) throw std::runtime_error (
        "в таблице " + name + " уже происходит чтение/запись\n"
    );

    ptrIter = new IterByPages {*this, fm};
}


void Table::delete_iter() const {
    delete (ptrIter);
    ptrIter = nullptr;
}


Table::Iterator Table::begin() const {
    create_iter();
    return IterImpl {const_cast<Table*> (this)};
}


Table::Iterator Table::end() const {
    return IterImpl {nullptr};
}

string const& Table::get_element (Table::Iterator it, string const& columnName) const {
    auto columnIt = algs::find (columns.begin(), columns.end(), columnName);

    if (columnIt == columns.end()) throw std::runtime_error (
        "в таблице \"" + name + "\""
      + " отсутствует колонка \""
      + columnName + "\"\n"
    );

    auto ind = columnIt - columns.begin();
    return (*it)[ind];
}


void Table::insert_back (Array const& newLine) {
    auto expected = columns.size() - 1;
    auto received = newLine.size();

    if (expected != received) throw std::runtime_error(
        "неверное количество элементов: "s
      + "ожидается - " + to_string (expected) + ", "
      + "переданно - " + to_string (received) + "\n"
    );

    if (fm.locked()) {
            //sleep
    }

    fm.lock();
    auto [pageNumb, lineNumb] = fm.get_position();
    auto primeKey = fm.get_prime_key();
        
    if (lineNumb >= linesLimit) {
        ++pageNumb;
        lineNumb = 0;
        fm.creat_page (pageNumb);
    }

    ++primeKey;
    ++lineNumb;

    ofstream page (fm.page_file_name (pageNumb), ios::app);
    page << primeKey << ",";
    fm.page_write_line (page, newLine);
    page.close();

    fm.set_position (pageNumb, lineNumb);
    fm.set_prime_key (primeKey);
    fm.unlock();
}


void Table::erase (Iterator it) {
    it.real()->erase();
}  

