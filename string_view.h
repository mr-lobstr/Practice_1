#ifndef STRING_VIEW_H_GUARD
#define STRING_VIEW_H_GUARD

#include <string>
#include <fstream>
#include <utility>
#include "data_struct/list.h"

class StringView {
public:
    StringView() noexcept  = default;
    ~StringView() noexcept = default;
    StringView (StringView const&) noexcept = default;
    StringView& operator= (StringView const&) noexcept = default;

    StringView (StringView&&) noexcept;
    StringView& operator= (StringView&&) noexcept;

    template <std::size_t N>
    explicit StringView (char const (&)[N]) noexcept;

    StringView (char const*, std::size_t) noexcept;
    StringView (std::string const&) noexcept;

    explicit operator std::string() const;

    char const* begin() const noexcept;
    char const* end() const noexcept;

    bool empty() const noexcept;
    std::size_t size() const noexcept;

    char const& operator[] (std::size_t) const noexcept;

    char const& front() const noexcept;
    char const& back() const noexcept;

private:
    char const* begView = nullptr;
    std::size_t size_ = 0;
};


bool operator== (StringView const&, StringView const&) noexcept;
bool operator!= (StringView const&, StringView const&) noexcept;


std::ostream& operator<< (std::ostream&, StringView const&);

data_struct::List<StringView> split (StringView const&, char);
data_struct::List<StringView> split_into_words (StringView const&);

std::string join_views (data_struct::List<StringView> const&);

#endif