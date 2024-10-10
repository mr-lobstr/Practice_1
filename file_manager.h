#ifndef TABLE_FILE_MANAGER_H_GUARD
#define TABLE_FILE_MANAGER_H_GUARD

#include <string>
#include <fstream>
#include <filesystem>
// #include "table.h"
#include "data_struct/dynamic_array.h"

class Table;

class TableFileManager {
    using Position = data_struct::Pair<std::size_t, std::size_t>;

public:
    TableFileManager (Table&, std::string const& path_ = "");
    ~TableFileManager() noexcept = default;

    void set_path_directory (std::string const&);

    bool locked() const;
    void lock() const;
    void unlock() const;

    size_t get_prime_key() const;
    void set_prime_key (std::size_t) const;

    Position get_position() const;
    void set_position (std::size_t, std::size_t) const;

    void create_files() const;

    std::string page_file_name (std::size_t) const;
    std::string pk_file_name() const;
    std::string lock_file_name() const;
    std::string pos_file_name() const;

    void creat_page (std::size_t) const;
    void delete_page (std::size_t numb) const;

    void page_write_line (std::ofstream&, data_struct::DynamicArray<std::string> const&) const;  

private:
    Table& table;
    std::string pathDir{};
};

#endif