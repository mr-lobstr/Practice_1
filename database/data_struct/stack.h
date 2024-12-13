#ifndef MY_STACK_H_GUARD
#define MY_STACK_H_GUARD

#include "dynamic_array.h"

namespace data_struct
{
    template <typename T>
    class Stack {
    public:
        using value_type = typename DynamicArray<T>::value_type;
        
        using iterator       = typename DynamicArray<T>::iterator;
        using const_iterator = typename DynamicArray<T>::const_iterator;

    public:
        Stack() noexcept = default;

        template <class Iter, class = iter::EnableIfForward<Iter>>
        Stack (Iter beg, Iter end)
            : impl (beg, end)
        {}

        Stack (std::initializer_list<T> iList)
            : impl (iList)
        {}

        Stack (std::size_t count, T const& value = T())
            : impl (count, value)
        {}

        friend
        bool operator== (Stack const& lhs, Stack const& rhs) noexcept {
            return lhs.impl == rhs.impl;
        }

        friend
        bool operator!= (Stack const& lhs, Stack const& rhs) noexcept {
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

        template <typename... Ts>
        void emplace (Ts&&... params) {
            impl.emplace_back (std::forward<Ts> (params)...);
        }

        void push (T const& value) {
            impl.push_back (value);
        }

        void push (T&& value) {
            impl.push_back (std::move (value));
        }

        T const& top() const noexcept {
            return impl.back();
        }

        T& top() noexcept {
            return impl.back();
        }

        void pop() noexcept {
            impl.pop_back();
        }

        bool empty() const noexcept {
            return impl.empty();
        }

        std::size_t size() const noexcept {
            return impl.size();
        }

        std::size_t capacity() const noexcept {
            return impl.capacity();
        }

        void reserve (std::size_t newCapacity) {
            impl.reserve (newCapacity);
        }

        void swap (Stack& rhs) noexcept {
            impl.swap (rhs.impl);
        }

    private:
        DynamicArray<T> impl;
    };


    template <typename T>
    void swap (Stack<T>& lhs, Stack<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif