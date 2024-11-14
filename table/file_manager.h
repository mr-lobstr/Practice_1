#ifndef TABLE_FILE_MANAGER_H_GUARD
#define TABLE_FILE_MANAGER_H_GUARD

#include <string>
#include <fstream>
#include <functional>
#include "table_fwd.h"


class TFileManager {
    using Position = ds::Pair<PageNumb, RowNumb>;
    using Self = TFileManager;

public:
    TFileManager (Table const&) noexcept;
    ~TFileManager() noexcept = default;

    Self& operator= (Self const&) = delete;
    Self& operator= (Self&&) = delete;

public:
    TMode get_mode() const;
    PrimeKey get_prime_key() const;
    Position get_position() const;

    void set_mode (TMode) const;
    void set_prime_key (PrimeKey) const;
    void set_position (PageNumb, RowNumb) const;

    bool create_table_dir() const;

    void creat_page (PageNumb) const;
    void delete_page (PageNumb) const;

    void write_page (PageNumb, std::function<void(std::ofstream&)>) const;
    void add_page (PageNumb, std::function<void(std::ofstream&)>) const;
    void read_page (PageNumb, std::function<void(std::ifstream&)>) const;

private:
    std::string page_file_path (PageNumb) const;
    std::string pk_file_path() const;
    std::string lock_file_path() const;
    std::string position_file_path() const;

private:
    friend TState;
    Table const& table;
};

#endif