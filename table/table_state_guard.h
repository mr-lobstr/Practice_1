#ifndef TABLE_PROCESS_GUARD_H_GUARD
#define TABLE_PROCESS_GUARD_H_GUARD

#include <cstdint>

class Table;


class TableStateGuard {
    using Self = TableStateGuard;

public:
    TableStateGuard (Table const&);
    ~TableStateGuard() noexcept;

    Self& operator= (Self const&) = delete;
    Self& operator= (Self&&) = delete;

    std::size_t current_pk() noexcept;
    std::size_t current_page() noexcept;
    std::size_t currnet_row() noexcept;

    void set_position (std::size_t, std::size_t) noexcept;
    void recording_start() noexcept;
    void recording_finish() noexcept;

    void update();

private:
    Table const& table;

    bool isUpdated = false;
    bool recording = false;

    std::size_t primeKey = 0;
    std::size_t page = 0;
    std::size_t row = 0;
};

#endif