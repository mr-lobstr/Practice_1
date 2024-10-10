#ifndef MY_stackImplH_GUARD
#define MY_stackImplH_GUARD

#include "dynamic_array.h"

namespace data_struct
{
    template <typename T>
    class Stack {
    public:
        using iterator       = typename DynamicArray<T>::iterator;
        using const_iterator = typename DynamicArray<T>::const_iterator;

    public:
        auto begin() const noexcept {
            return stackImpl.cbegin();
        }

        auto end() const noexcept {
            return stackImpl.cend();
        }

        auto cbegin() const noexcept {
            return stackImpl.cbegin();
        }

        auto cend() const noexcept {
            return stackImpl.cend();
        }

        template <typename... Ts>
        void emplace (Ts&&... params) {
            stackImpl.emplace_back (std::forward<Ts> (params)...);
        }

        void push (T const& value) {
            stackImpl.push_back (value);
        }

        void push (T&& value) {
            stackImpl.push_back (std::move (value));
        }

        T const& top() const noexcept {
            return stackImpl.back();
        }

        void pop() noexcept {
            stackImpl.pop_back();
        }

        bool empty() const noexcept {
            return stackImpl.empty();
        }

        std::size_t size() const noexcept {
            return stackImpl.size();
        }

        std::size_t capacity() const noexcept {
            return stackImpl.capacity();
        }

        void reserve (std::size_t newCapacity) {
            stackImpl.reserve (newCapacity);
        }

    private:
        DynamicArray<T> stackImpl;
    };


    template <typename T>
    void swap (Stack<T>& lhs, Stack<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif