#include "file_manager.h"
#include "table.h"

using namespace std;
using namespace filesystem;
using namespace data_struct;


TableFileManager::TableFileManager (Table& table_, string const &path_)
    : table (table_)
    , pathDir (path_)
{}

void TableFileManager::set_path_directory (std::string const& path_) {
    pathDir = path_;
}

template <typename... Ts>
void read (ifstream& file, Ts&...inputs) {
    (file >> ... >> inputs);
}


template <typename... Ts>
void write (ofstream& file, Ts const&... outputs) {
    (file << ... << outputs);
}


template <typename... Ts>
void read_file (string const &fileName, Ts &...inputs) {
    ifstream file (fileName);

    if (not file.open()) throw std::runtime_error (
        "не удалось открыть файл \"" + fileName + "\"\n"
    );

    read (file, inputs...);
    file.close();
}


template <typename Mode, typename... Ts>
void write_file(string const &fileName, Mode mode, Ts const &...outputs) {
    ofstream file (fileName, mode);

    if (not file.open()) throw std::runtime_error (
        "не удалось открыть файл \"" + fileName + "\"\n"
    );

    write (file, outputs...);
    file.close();
}


bool TableFileManager::locked() const {
    bool is_lock;
    read_file (lock_file_name(), is_lock);
    return is_lock;
}


void TableFileManager::lock() const {
    write_file (lock_file_name(), ios::trunc, 1);
}


void TableFileManager::unlock() const {
    write_file (lock_file_name(), ios::trunc, 0);
}


size_t TableFileManager::get_prime_key() const {
    size_t prime_key;
    read_file (pk_file_name(), prime_key);
    return prime_key;
}


void TableFileManager::set_prime_key (size_t pk) const {
    write_file (pk_file_name(), ios::trunc, pk);
}


TableFileManager::Position TableFileManager::get_position() const {
    size_t pageNumb, currLine;
    read_file (pos_file_name(), pageNumb, currLine);
    return {pageNumb, currLine};
}


void TableFileManager::set_position (size_t pageNumb, size_t line) const {
    write_file (pos_file_name(), ios::trunc, pageNumb, " ", line);
}


string TableFileManager::page_file_name (size_t numb) const {
    return pathDir + to_string (numb) + ".csv";
}


void TableFileManager::create_files() const {
    creat_page (1);
    set_position (1, 0);
    set_prime_key (0);
    unlock(); 
}


string TableFileManager::pk_file_name() const {
    return pathDir + table.name + "_pk_sequence";
}


string TableFileManager::lock_file_name() const {
    return pathDir + table.name + "_lock";
}


string TableFileManager::pos_file_name() const {
    return pathDir + table.name + "_current_page";
}


void TableFileManager::page_write_line (ofstream &page, DynamicArray<string> const &line) const {
    bool is_first = true;
    for (auto &el : line) {
        page << (is_first ? "" : ",") << el;
        is_first = false;
    }
    page << "\n";
}


void TableFileManager::creat_page (size_t numb) const {
    ofstream page (page_file_name (numb));
    page_write_line (page, table.columns);
    page.close();
}


void TableFileManager::delete_page (size_t numb) const {
    remove (page_file_name (numb));
}