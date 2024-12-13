#include <filesystem>
#include "file_manager.h"
#include "table.h"

using namespace std;
using namespace filesystem;
using namespace data_struct;

using TFM = TFileManager;


TFM::TFileManager (Table const& table_) noexcept
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


TMode TFM::get_mode() const {
    int mode;
    FReader fileLock (lock_file_path());
    fileLock >> mode;
    return static_cast<TMode> (mode);
}


void TFM::set_mode (TMode mode) const {
    FWriter fileLock (lock_file_path());
    fileLock << (int) mode;
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


bool TFM::create_table_dir() const {
    return create_directory (table.path_dir());
}


string create_table_header (TableName name, HashTable<Column, ColumnNumb> const& columns) {
    DynamicArray<Column const*> ptrsColumns (columns.size());

    for (auto& [column, index] : columns) {
        ptrsColumns[index-1] = &column;
    }

    string header = name + "_pk";

    for (auto& pColumn : ptrsColumns) {
        header += ("," + *pColumn);
    }

    return header;
}


void TFM::creat_page (size_t numb) const {
    FWriter filePage (page_file_path (numb));
    filePage << create_table_header (table.name, table.columns) << "\n";
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