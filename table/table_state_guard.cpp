#include <iostream>
#include "table_state_guard.h"
#include "file_manager.h"
#include "table.h"
using namespace std;

using TSG = TableStateGuard;

TSG::TableStateGuard (Table const& table_)
    : table (table_)
{
    if (table.fm.is_locked()) {
        throw runtime_error ("уже происходит чтение/запись\n");
    }
    
    try {
        table.fm.lock();
        primeKey = table.fm.get_prime_key();
        auto pos = table.fm.get_position();

        page = pos.first;
        row = pos.second;
    } catch (...) {
        table.fm.unlock();
        throw;
    }
}


TSG::~TableStateGuard() {
    if (isUpdated)
        return;

    try {
        update();
    } catch (std::runtime_error const& e) {
        std::cerr << e.what();
        std::terminate();
    }
}


void TSG::update() {
    if (recording) {
        if (row == 1) {
            row = table.rows_limit();
            --page;
        } else {
            --row;
        }

        --primeKey;
    }
    
    table.fm.set_position (page, row);
    table.fm.set_prime_key (primeKey);
    table.fm.unlock();

    isUpdated = true;
}


std::size_t TSG::current_pk() noexcept {
    return primeKey;
}


std::size_t TSG::current_page() noexcept {
    return page;
}


std::size_t TSG::currnet_row() noexcept {
    return row;
}


void TSG::recording_start() noexcept {
    recording = true;

    ++primeKey;

    if (row == table.rows_limit()) {
        row = 1;
        table.fm.creat_page (++page);
    } else {
        ++row;
    }
}


void TSG::recording_finish() noexcept {
    recording = false;
}


void TSG::set_position (size_t page_, size_t row_) noexcept {
    page = page_;
    row = row_;
}
