#ifndef TABLE_FILE_MANAGER_H_GUARD
#define TABLE_FILE_MANAGER_H_GUARD

#include <string>
#include <fstream>
#include <functional>
#include "data_struct/my_algorithm.h"

class Table;


class TableFileManager {
    using Position = data_struct::Pair<std::size_t, std::size_t>;
    using Self = TableFileManager;

public:
    TableFileManager (Table&) noexcept;
    ~TableFileManager() noexcept = default;

    Self& operator= (Self const&) = delete;
    Self& operator= (Self&&) = delete;

    bool is_locked() const;
    void lock() const;
    void unlock() const;

    size_t get_prime_key() const;
    Position get_position() const;
    void set_prime_key (std::size_t) const;
    void set_position (std::size_t, std::size_t) const;

    void create_files() const;

    void creat_page (std::size_t) const;
    void write_page (std::size_t, std::function<void(std::ofstream&)>) const;
    void add_page (std::size_t, std::function<void(std::ofstream&)>) const;
    void read_page (std::size_t, std::function<void(std::ifstream&)>) const;
    void delete_page (std::size_t) const;

private:
    std::string page_file_name (std::size_t) const;
    std::string pk_file_name() const;
    std::string lock_file_name() const;
    std::string position_file_name() const;

private:
    friend class TableStateGuard;

    Table& table;
};

#endif