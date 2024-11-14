#ifndef TABLE_STATE_GUARD_H_GUARD
#define TABLE_STATE_GUARD_H_GUARD

#include <mutex>
#include <condition_variable>
#include "table_fwd.h"

class TState {
public:
    TState (Table const&);
    ~TState() noexcept;

    TState& operator= (TState const&) = delete;
    TState& operator= (TState&&) = delete;

    TMode mode() const noexcept;

    PrimeKey current_pk() const noexcept;
    PageNumb current_page() const noexcept;
    RowNumb  currnet_row() const noexcept;

    void set_position (PageNumb, RowNumb) noexcept;
    void row_writing_start() noexcept;
    void row_writing_finish() noexcept;
    
    void init() const;
    void sync();

    void reading_start();
    void reading_finish();
    void writing_start();
    void writing_finish();

private:
    Table const& table;

    mutable TMode mode_ = TMode::free;

    mutable PrimeKey pk = 0;
    mutable PageNumb page = 1;
    mutable RowNumb  row = 0;

    bool rowWriting = false;
    bool synchronized = true;

    mutable std::mutex mtx{};
    mutable std::condition_variable cv{};
    mutable std::size_t readerCnt = 0;
};


class TStateGuard {
public:
    TStateGuard (TState&, TMode);
    ~TStateGuard();

    TMode mode() const noexcept;

private:
    TState& state;
    TMode mode_;
};

#endif