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

template <typename FStream>
struct FileProcess {
    FileProcess (std::string const& fileName, std::ios::openmode mode)
        : file (fileName, mode)
    {
        if (not file.is_open()) throw std::runtime_error (
            "не удалось открыть файл \"" + fileName + "\"\n"
        );
    }

    ~FileProcess() {
        file.close();
    }

    FStream file;
};


struct FReader
    : public FileProcess<std::ifstream>
{
    FReader (std::string const& fileName)
        : FileProcess (fileName, std::ios::in)
    {}

    template <typename T>
    FReader& operator>> (T& value) {
        file >> value;
        return *this;
    }
};


struct FWriter:
    public FileProcess<std::ofstream>
{
    FWriter (std::string const& fileName)
        : FileProcess (fileName, std::ios::trunc)
    {}

    template <typename T>
    FWriter& operator<< (T const& value) {
        file << value;
        return *this;
    }
};


struct FAdder
    : public FileProcess<std::ofstream>
{
    FAdder (std::string const& fileName)
        : FileProcess (fileName, std::ios::app)
    {}

    template <typename T>
    FAdder& operator<< (T const& value) {
        file << value;
        return *this;
    }
};


bool TFM::is_locked() const {
    bool isLocked;
    FReader fileLock (lock_file_path());
    fileLock >> isLocked;
    return isLocked;
}


void TFM::lock() const {
    FWriter fileLock (lock_file_path());
    fileLock << Table::lock;
}


void TFM::unlock() const {
    FWriter fileLock (lock_file_path());
    fileLock << Table::unlock;
}


size_t TFM::get_prime_key() const {
    size_t primeKey;
    FReader filePk (pk_file_path());
    filePk >> primeKey;
    return primeKey;
}


void TFM::set_prime_key (size_t pk) const {
    FWriter filePk (pk_file_path());
    filePk << pk;
}


TFM::Position TFM::get_position() const {
    size_t page, row;
    FReader filePos (position_file_path());
    filePos >> page >> row;
    return {page, row};
}


void TFM::set_position (size_t page, size_t row) const {
    FWriter filePos (position_file_path());
    filePos << page << " " << row;
}


string TFM::page_file_path (size_t numb) const {
    return table.path_dir() + "/" + to_string (numb) + ".csv";
}


string TFM::pk_file_path() const {
    return table.path_dir() + "/" + table.name + "_pk_sequence";
}


string TFM::lock_file_path() const {
    return table.path_dir() + "/" + table.name + "_lock";
}


string TFM::position_file_path() const {
    return table.path_dir() + "/" + table.name + "_current_page";
}


void TFM::create_files() const {
    create_directory (table.path_dir());
    creat_page (1);
    set_position (1, 0);
    set_prime_key (0);
    unlock();
}


void TFM::creat_page (size_t numb) const {
    FWriter filePage (page_file_path (numb));
    filePage << table.header() << "\n";
}


void TFM::add_page (size_t numb, function<void(ofstream&)> addFunc) const {
    FAdder filePage (page_file_path (numb));
    addFunc (filePage.file);
}


void TFM::write_page (size_t numb, function<void(ofstream&)> addFunc) const {
    creat_page (numb);
    FAdder filePage (page_file_path (numb));
    addFunc (filePage.file);
}


void TFM::read_page (size_t numb, function<void(ifstream&)> readFunc) const {
    FReader filePage (page_file_path (numb));
    readFunc (filePage.file);
}


void TFM::delete_page (size_t numb) const {
    remove (page_file_path (numb));
}