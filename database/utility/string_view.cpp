#include "string_view.h"
#include "../data_struct/my_algorithm.h"
#include "../data_struct/dynamic_array.h"
using namespace std;
using namespace data_struct;

SView::SView (SView&& rhs) noexcept
    : begView (exchange (rhs.begView, nullptr))
    , size_ (exchange (rhs.size_, 0))
{}


SView& SView::operator= (SView&& rhs) noexcept {
    begView = exchange (rhs.begView, nullptr);
    size_ = exchange (rhs.size_, 0);
    return *this;
}


SView::SView (char const* ptr, size_t sz) noexcept
    : begView (ptr)
    , size_ (sz)
{}


SView::SView (string const& str) noexcept
    : begView (str.data())
    , size_ (str.size())
{}


SView::SView (string::const_iterator beg, string::const_iterator end) noexcept
    : begView (beg.base())
    , size_ (end - beg)
{}


SView::operator string() const {
    return string (begin(), size());
}


string operator+ (string str, SView sv) {
    return str += sv; 
}


string operator+ (SView sv, string str) {
    return str += sv; 
}


bool operator== (SView const& lhs, SView const& rhs) noexcept {
    if (lhs.size() != rhs.size())
        return false;

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}


bool operator!= (SView const& lhs, SView const& rhs) noexcept {
    return not (lhs == rhs);
}


char const* SView::begin() const noexcept {
    return begView;
}


char const* SView::end() const noexcept {
    return begView + size_;
}


bool SView::empty() const noexcept {
    return size_ == 0;
}


size_t SView::size() const noexcept {
    return size_;
}


char const& SView::operator[] (size_t ind) const noexcept {
    return begView[ind];
}


char const& SView::front() const noexcept {
    return *begin();
}


char const& SView::back() const noexcept {
    return *(end() - 1);
}


void SView::shorten_left (size_t n) noexcept {
    begView += n;
    size_ -= n;
}


void SView::shorten_right (size_t n) noexcept {
    size_ -= n;
}


ostream& operator<< (ostream& out, SView const& sv) {
    for (char c : sv) {
        out << c;
    }
    return out;
}


DynamicArray<SView> sv_split (SView const& sv, char separator) {
    DynamicArray<SView> views;
    auto subBeg = sv.begin();

    while (true) {
        auto subEnd = algs::find (subBeg, sv.end(), separator);
        views.emplace_back (subBeg, subEnd - subBeg);

        if (subEnd == sv.end())
            break;

        subBeg = subEnd + 1;
    }

    return views;
}


DynamicArray<SView> split_into_words (SView const& sv) {
    DynamicArray<SView> views;
    auto subBeg = sv.begin();
    auto subEnd = sv.begin();

    while (subEnd != sv.end()) {
        subBeg = algs::find_if (subEnd, sv.end(), [&] (char c) {
            return not isspace (c);
        });

        if (subBeg == sv.end())
            break;

        subEnd = algs::find_if (subBeg, sv.end(), (int(*)(int)) isspace);
        views.emplace_back (subBeg, subEnd - subBeg);
    }

    return views;
}


string join_views (DynamicArray<SView> const& views, string const& separator) {   
    auto str = ""s;
    bool isFirst = true;

    for (auto& view : views) {
        str += (isFirst ? "" : separator);
        str += view;
        isFirst = false;
    }

    return str;
}