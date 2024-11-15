#include "table_state.h"
#include "file_manager.h"
#include "table.h"
using namespace std;

TState::TState (Table const& table_)
    : table (table_)
{
    if (table.fm.create_table_dir()) {
        table.fm.set_position (page, row);
        table.fm.set_prime_key (pk);
        table.fm.set_mode (TMode::free);
    } else {
        auto [p, r] = table.fm.get_position();
        set_position (p, r);
        
        pk = table.fm.get_prime_key();
        mode_ = table.fm.get_mode();
    }
}


TState::~TState() {
    try {
        sync();
    } catch (std::runtime_error const& e) {
        std::cerr << e.what();
        std::terminate();
    }
}


void TState::sync() {
    if (synchronized)
        return;

    if (rowWriting) {
        if (row == 1) {
            row = table.rows_limit();
            --page;
        } else {
            --row;
        }

        --pk;
    }
    
    table.fm.set_position (page, row);
    table.fm.set_prime_key (pk);

    synchronized = true;
}


TMode TState::mode() const noexcept {
    return mode_;
}


std::size_t TState::current_pk() const noexcept {
    return pk;
}


std::size_t TState::current_page() const noexcept {
    return page;
}


std::size_t TState::currnet_row() const noexcept {
    return row;
}


void TState::row_writing_start() noexcept {
    rowWriting = true;

    ++pk;

    if (row == table.rows_limit()) {
        row = 1;
        table.fm.creat_page (++page);
    } else {
        ++row;
    }
}


void TState::row_writing_finish() noexcept {
    rowWriting = false;
    synchronized = false;
}


void TState::set_position (size_t page_, size_t row_) noexcept {
    page = page_;
    row = row_;

    synchronized = false;
}


void TState::reading_start() {
    std::unique_lock<std::mutex> lock (mtx);

    cv.wait (lock, [this] { 
        return mode_ != TMode::writing;
    });

    if (mode_ == TMode::free) {
        mode_ = TMode::reading;
        table.fm.set_mode (mode_);
    }

    ++readerCnt;
}


void TState::reading_finish() {
    std::unique_lock<std::mutex> lock (mtx);

    --readerCnt;

    if (readerCnt == 0) {
        mode_ = TMode::free;
        table.fm.set_mode (mode_);

        cv.notify_one();
    }
}


void TState::writing_start() {
    std::unique_lock<std::mutex> lock (mtx);

    cv.wait (lock, [this] { 
        return mode_ == TMode::free;
    });

    mode_ = TMode::writing;
    table.fm.set_mode (mode_);
}


void TState::writing_finish() {
    std::unique_lock<std::mutex> lock (mtx);

    mode_ = TMode::free;
    table.fm.set_mode (mode_);

    sync();
    cv.notify_one();
}


TStateGuard::TStateGuard (TState& state_, TMode m)
    : state (state_)
    , mode_ (m)
{
    if (mode_ == TMode::reading) {
        state.reading_start();
    } else if (mode_ == TMode::writing) {
        state.writing_start();
    }
}


TMode TStateGuard::mode() const noexcept {
    return mode_;
}


TStateGuard::~TStateGuard() {
    if (mode_ == TMode::reading) {
        state.reading_finish();
    } else if (mode_ == TMode::writing) {
        state.writing_finish();
    }
}