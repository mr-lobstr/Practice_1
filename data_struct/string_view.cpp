#include "string_view.h"
#include "my_algorithm.h"
using namespace std;
using namespace data_struct;

StringView::StringView (StringView&& rhs) noexcept
    : begView (exchange (rhs.begView, nullptr))
    , size_ (exchange (rhs.size_, 0))
{}


StringView& StringView::operator= (StringView&& rhs) noexcept {
    begView = exchange (rhs.begView, nullptr);
    size_ = exchange (rhs.size_, 0);
    return *this;
}


StringView::StringView (char const* ptr, size_t sz) noexcept
    : begView (ptr)
    , size_ (sz)
{}


StringView::StringView (string const& str) noexcept
    : begView (str.data())
    , size_ (str.size())
{}


StringView::operator string() const {
    return string (begin(), size());
}


bool operator== (StringView const& lhs, StringView const& rhs) noexcept {
    if (lhs.size() != rhs.size())
        return false;

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}


bool operator!= (StringView const& lhs, StringView const& rhs) noexcept {
    return not (lhs == rhs);
}


char const* StringView::begin() const noexcept {
    return begView;
}


char const* StringView::end() const noexcept {
    return begView + size_;
}


bool StringView::empty() const noexcept {
    return size_ == 0;
}


size_t StringView::size() const noexcept {
    return size_;
}


char const& StringView::operator[] (size_t ind) const noexcept {
    return begView[ind];
}


char const& StringView::front() const noexcept {
    return *begin();
}


char const& StringView::back() const noexcept {
    return *(end() - 1);
}


void StringView::shorten_left (size_t n) noexcept {
    begView += n;
    size_ -= n;
}


void StringView::shorten_right (size_t n) noexcept {
    size_ -= n;
}


ostream& operator<< (ostream& out, StringView const& sv) {
    for (char c : sv) {
        out << c;
    }
    return out;
}


DynamicArray<StringView> split (StringView const& sv, char separator) {
    DynamicArray<StringView> views;
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


DynamicArray<StringView> split_into_words (StringView const& sv) {
    DynamicArray<StringView> views;
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


string join_views (DynamicArray<StringView> const& views) {   
    auto str = ""s;

    for (auto& view : views) {
        str += string (view);
    }

    return str;
}