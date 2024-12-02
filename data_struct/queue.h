#ifndef MY_QUEUE_H_GUARD
#define MY_QUEUE_H_GUARD

#include "list.h"

namespace data_struct
{
    template <typename T>
    class Queue {
    public:
        using value_type = typename List<T>::value_type;

        using iterator       = typename List<T>::iterator;
        using const_iterator = typename List<T>::const_iterator;

    public:
        Queue() noexcept = default;

        template <class Iter, class = iter::EnableIfForward<Iter>>
        Queue (Iter beg, Iter end)
            : impl (beg, end)
        {}

        Queue (std::initializer_list<T> iList)
            : impl (iList)
        {}

        Queue (std::size_t count, T const& value = T())
            : impl (count, value)
        {}

        friend
        bool operator== (Queue const& lhs, Queue const& rhs) noexcept {
            return lhs.impl == rhs.impl;
        }

        friend
        bool operator!= (Queue const& lhs, Queue const& rhs) noexcept {
            return not (lhs == rhs);
        }
        auto begin() const noexcept {
            return impl.cbegin();
        }

        auto end() const noexcept {
            return impl.cend();
        }

        auto cbegin() const noexcept {
            return impl.cbegin();
        }

        auto cend() const noexcept {
            return impl.cend();
        }

        bool empty() const noexcept {
            return impl.empty();
        }

        std::size_t size() const noexcept {
            return impl.size();
        }

        void swap (Queue& rhs) noexcept {
            impl.swap (rhs.impl);
        }

        template <typename... Ts>
        void emplace_back (Ts&&... params) {
            impl.emplace_back (std::forward<Ts> (params)...);
        }

        void push_back (T const& value) {
            impl.push_back (value);
        }

        void push_back (T&& value) {
            impl.push_back (std::move (value));
        }

        void pop_front() noexcept {
            impl.pop_front();
        }

        T const& front() const noexcept {
            return impl.front();
        }

        T const& back() const noexcept {
            return impl.back();
        }

    private:
        List<T> impl{};
    };


    template <typename T>
    void swap (Queue<T>& lhs, Queue<T>& rhs) noexcept {
        lhs.swap (rhs);
    }
}
#endif