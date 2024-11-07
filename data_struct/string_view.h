#ifndef STRING_VIEW_H_GUARD
#define STRING_VIEW_H_GUARD

#include <string>
#include <fstream>
#include <utility>
#include "dynamic_array.h"

class StringView {
public:
    StringView() noexcept  = default;
    ~StringView() noexcept = default;
    StringView (StringView const&) noexcept = default;
    StringView& operator= (StringView const&) noexcept = default;

    StringView (StringView&&) noexcept;
    StringView& operator= (StringView&&) noexcept;

    StringView (char const*, std::size_t) noexcept;
    StringView (std::string const&) noexcept;
    StringView (std::string::const_iterator, std::string::const_iterator) noexcept;

    operator std::string() const;

    friend std::string operator+ (std::string, StringView);
    friend std::string operator+ (StringView, std::string);

    char const* begin() const noexcept;
    char const* end() const noexcept;

    bool empty() const noexcept;
    std::size_t size() const noexcept;

    char const& operator[] (std::size_t) const noexcept;

    char const& front() const noexcept;
    char const& back() const noexcept;

    void shorten_left (std::size_t) noexcept;
    void shorten_right (std::size_t) noexcept;

private:
    char const* begView = nullptr;
    std::size_t size_ = 0;
};


bool operator== (StringView const&, StringView const&) noexcept;
bool operator!= (StringView const&, StringView const&) noexcept;


std::ostream& operator<< (std::ostream&, StringView const&);

data_struct::DynamicArray<StringView> split (StringView const&, char);
data_struct::DynamicArray<StringView> split_into_words (StringView const&);

std::string join_views (data_struct::DynamicArray<StringView> const&, std::string const&);

#endif