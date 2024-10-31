#include <filesystem>
#include "file_manager.h"
#include "table.h"

using namespace std;
using namespace filesystem;
using namespace data_struct;

using TFM = TableFileManager;


TFM::TableFileManager (Table& table_) noexcept
    : table (table_)
{}


template <typename Stream, typename Process>
void process_file (std::string const& fileName, std::ios::openmode mode, Process process) {
    Stream file (fileName, mode);

    if (not file.is_open()) throw std::runtime_error (
        "не удалось открыть файл \"" + fileName + "\"\n"
    );

    process (file);
    file.close();
}

template <typename Read>
void read_file (std::string const& fileName, Read rFunc) {
    process_file<std::ifstream> (fileName, std::ios::in, rFunc);
}


template <typename Rewrite>
void rewrite_file (std::string const& fileName, Rewrite rwFunc) {
    process_file<std::ofstream> (fileName, std::ios::trunc, rwFunc);
}


template <typename Add>
void add_file (std::string const& fileName, Add addFunc) {
    process_file<std::ofstream> (fileName, std::ios::app, addFunc);
}


bool TFM::is_locked() const {
    bool isLocked;
    read_file (lock_file_name(), [&] (auto& fLock) {
        fLock >> isLocked;
    });
    return isLocked;
}


void TFM::lock() const {
    rewrite_file (lock_file_name(), [] (auto& fLock) {
        fLock << Table::lock;
    });
}


void TFM::unlock() const {
    rewrite_file (lock_file_name(), [] (auto& fLock) {
        fLock << Table::unlock;
    });
}


size_t TFM::get_prime_key() const {
    size_t prime_key;
    read_file (pk_file_name(), [&] (auto& fPrimeKey) {
        fPrimeKey >> prime_key;
    });
    return prime_key;
}


void TFM::set_prime_key (size_t pk) const {
    rewrite_file (pk_file_name(), [&] (auto& fPrimeKey) {
        fPrimeKey << pk;
    });
}


TFM::Position TFM::get_position() const {
    size_t page, row;
    read_file (position_file_name(), [&] (auto& fPos) {
        fPos >> page >> row;
    });
    return {page, row};
}


void TFM::set_position (size_t page, size_t row) const {
    rewrite_file (position_file_name(), [&] (auto& fPos) {
        fPos << page << " " << row;
    });
}


string TFM::page_file_name (size_t numb) const {
    return table.pathDir + to_string (numb) + ".csv";
}



string TFM::pk_file_name() const {
    return table.pathDir + table.name + "_pk_sequence";
}


string TFM::lock_file_name() const {
    return table.pathDir + table.name + "_lock";
}


string TFM::position_file_name() const {
    return table.pathDir + table.name + "_current_page";
}


void TFM::create_files() const {
    creat_page (1);
    set_position (1, 0);
    set_prime_key (0);
    unlock();
}


void TFM::creat_page (size_t numb) const {
    rewrite_file (page_file_name (numb), [&] (auto& fPage) {
        fPage << table.header() << "\n";
    });
}


void TFM::add_page (size_t numb, function<void(ofstream&)> addFunc) const {
    add_file (page_file_name (numb), [&] (auto& fPage) {
        addFunc (fPage);
    });
}


void TFM::write_page (size_t numb, function<void(ofstream&)> wFunc) const {
    creat_page (numb);

    add_file (page_file_name (numb), [&] (auto& fPage) {
        wFunc (fPage);
    });
}


void TFM::read_page (size_t numb, function<void(ifstream&)> rFunc) const {
    read_file (page_file_name (numb), [&] (auto& fPage) {
        rFunc (fPage);
    });
}


void TFM::delete_page (size_t numb) const {
    remove (page_file_name (numb));
}