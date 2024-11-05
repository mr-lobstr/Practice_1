#include <utility>
#include "iterator_by_rows_impl.cpp"
using namespace std;

IteratorByRows::IteratorByRows (Table const& table)
    : pimpl (new IterImpl (table))
{}


IteratorByRows::IteratorByRows (IteratorByRows&& rhs)
    : pimpl (exchange (rhs.pimpl, nullptr))
{}


IteratorByRows::~IteratorByRows() noexcept
{
    delete (pimpl);
}


IteratorByRows& IteratorByRows::operator= (IteratorByRows&& rhs) {
    if (this != &rhs) {
        reset();
        swap (pimpl, rhs.pimpl);
    }
    return *this;
}


bool operator== (IteratorByRows const& lhs, IteratorByRows const& rhs) noexcept {
    if (lhs.is_end() or rhs.is_end())
        return lhs.is_end() and rhs.is_end();

    return false;
}


bool operator!= (IteratorByRows const& lhs, IteratorByRows const& rhs) noexcept {
    return not (lhs == rhs);
}


IteratorByRows& IteratorByRows::operator++() {
    if (not is_end()) {
        pimpl->next();

        if (pimpl->is_end()) {
            reset();
        }
    }
    return *this;
}


void IteratorByRows::erase() {
    if (not is_end()) {
        pimpl->erase();
    }
}


bool IteratorByRows::is_end() const noexcept {
    return pimpl == nullptr
        or pimpl->is_end();
}


void IteratorByRows::reset() noexcept {
    delete (pimpl);
    pimpl = nullptr;
}


StringView IteratorByRows::operator[] (string const& column) const {
    if (not is_end()) {
        return pimpl->get_row_element (column);
    }
    return {};
}


string const& IteratorByRows::operator*() const noexcept {
    return pimpl->get_row();
}