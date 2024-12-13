#ifndef MY_STRING_VIEW_H_GUARD
#define MY_STRING_VIEW_H_GUARD

#include <string>
#include <fstream>
#include <utility>

class SView {
public:
    SView() noexcept  = default;
    ~SView() noexcept = default;
    SView (SView const&) noexcept = default;
    SView& operator= (SView const&) noexcept = default;

    SView (SView&&) noexcept;
    SView& operator= (SView&&) noexcept;

    template <std::size_t N>
    SView (const char (&)[N]) noexcept;

    SView (char const*, std::size_t) noexcept;
    SView (std::string const&) noexcept;
    SView (std::string::const_iterator, std::string::const_iterator) noexcept;

    operator std::string() const;

    friend std::string operator+ (std::string, SView);
    friend std::string operator+ (SView, std::string);

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


template <std::size_t N>
SView::SView (const char (&arr)[N]) noexcept
    : SView (arr, N)
{}


bool operator== (SView const&, SView const&) noexcept;
bool operator!= (SView const&, SView const&) noexcept;


std::ostream& operator<< (std::ostream&, SView const&);

#endif