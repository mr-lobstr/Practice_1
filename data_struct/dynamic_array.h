#ifndef DYNAMIC_ARRAY_GUARD_H
#define DYNAMIC_ARRAY_GUARD_H

#include <stdexcept>
#include <utility>
#include "iterators.h"
#include "my_algorithm.h"

namespace array_detail
{
    template <typename T, typename C>
    struct IterImpl {
        using Container = C;

    public:
        IterImpl() noexcept = default;
        ~IterImpl() noexcept = default;
        IterImpl (IterImpl const&) noexcept = default;
        IterImpl& operator= (IterImpl const&) noexcept = default;

        IterImpl (IterImpl&& rhs) noexcept
            : ptr (std::exchange (rhs.ptr, nullptr))
        {}

        IterImpl& operator= (IterImpl&& rhs) noexcept {
            ptr = std::exchange (rhs.ptr, nullptr);
            return *this;
        }

        IterImpl (T* ptr_) noexcept
            : ptr (ptr_)
        {}

    public:
        T*& real() const noexcept {
            return ptr;
        }

        bool equal (IterImpl const& rhs) const noexcept {
            return ptr == rhs.ptr; 
        }

        auto diff (IterImpl const& rhs) const noexcept {
            return ptr - rhs.ptr; 
        }

        void plus (std::ptrdiff_t n) noexcept {
            ptr += n; 
        }

        void next() noexcept {
            ++ptr; 
        }

        void prev() noexcept {
            --ptr; 
        }

        T& get_value() const noexcept {
            return *ptr; 
        }

    public:
        mutable T* ptr{};
    };
}

namespace data_struct
{
    template <typename T>
    class DynamicArray {
        using IterImpl = array_detail::IterImpl<T, DynamicArray>;

    public:
        using iterator       = RandomIterator<T, IterImpl, Mutable_tag>;
        using const_iterator = RandomIterator<T, IterImpl, Const_tag>;

    public:
        DynamicArray() noexcept = default;

        DynamicArray (DynamicArray&& rhs) noexcept
            : capacity_ (std::exchange (rhs.capacity_, 0))
            , begin_ (std::exchange (rhs.begin_, nullptr))
            , end_ (std::exchange (rhs.end_, nullptr))
        {}

        template <class Iter, class = EnableIfForward<Iter>>
        DynamicArray (Iter beg, Iter end)
            : DynamicArray ()
        {
            algs::copy (beg, end, algs::back_inserter (*this));
        }

        DynamicArray (std::initializer_list<T> iList)
            : DynamicArray (iList.size(), InitTag{})
        {
            algs::copy (iList.begin(), iList.end(), algs::back_inserter (*this));
        }

        DynamicArray (DynamicArray const& rhs)
            : DynamicArray (rhs.size(), InitTag{})
        {
            algs::copy (rhs.begin(), rhs.end(), algs::back_inserter (*this));
        }

        DynamicArray (std::size_t count, T const& value = T())
            : DynamicArray (count, InitTag{})
        {
            while (count--) {
                push_back (value);
            }
        }

        DynamicArray& operator= (DynamicArray&& rhs) noexcept {
            if (this != &rhs) {
                auto tmp {std::move (rhs)};
                swap (tmp);
            }
            return *this;
        }

        DynamicArray& operator= (DynamicArray const& rhs) {
            if (this != &rhs) {
                auto tmp {rhs};
                swap (tmp);
            }
            return *this;
        }

        ~DynamicArray() noexcept {
            resize(0);
            mem_free (begin_);
        }

        auto begin() noexcept {
            return iterator {begin_};
        }

        auto cbegin() const noexcept {
            return const_iterator {no_const (begin_)};
        }

        auto begin() const noexcept {
            return cbegin();
        }

        auto end() noexcept {
            return iterator {end_};
        }

        auto cend() const noexcept {
            return const_iterator {no_const (end_)};
        }

        auto end() const noexcept {
            return cend();
        }

        T& operator[] (std::size_t ind) noexcept {
            return begin()[ind];
        }

        T const& operator[] (std::size_t ind) const noexcept {
            return begin()[ind];
        }

        T& front () noexcept {
            return *begin();
        }

        T const& front () const noexcept {
            return *begin();
        }

        T& back () noexcept {
            return *--end();
        }

        T const& back () const noexcept {
            return *--end();
        }

        void swap (DynamicArray& rhs) noexcept {
            std::swap (capacity_, rhs.capacity_);
            std::swap (begin_, rhs.begin_);
            std::swap (end_, rhs.end_);
        }

        std::size_t size() const noexcept {
            return end() - begin();
        }

        std::size_t capacity() const noexcept {
            return capacity_;
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        template <typename... Ts>
        void emplace_back (Ts&&... args) {
            reserve_before_insert();
            new(end_) T {std::forward<Ts> (args)...};
            ++end_;
        }

        void push_back (T const& value) {
            emplace_back (value);
        }

        void push_back (T&& value) {
            emplace_back (std::move (value));
        }

        template <typename... Ts>
        iterator emplace (const_iterator it, Ts&&... args) {
            auto diff = it - cbegin();
            push_back (std::move (back()));
            auto saveIt = begin() + diff;

            algs::shift_right (saveIt, end());
            new (saveIt.real()) T {std::forward<Ts> (args)...};

            return saveIt;
        }

        iterator insert (const_iterator it, T const& value) {
            return emplace (it, value);
        }

        iterator insert (const_iterator it, T&& value) {
            return emplace (it, std::move (value));
        }

        void pop_back() noexcept {
            --end_;
            end_->~T();
        }

        void erase (const_iterator it) {
            auto noConstIt = iterator (it.real());
            algs::shift_left (noConstIt, end());
            pop_back();
        }

        void reserve (std::size_t newCapacity) {
            realloc_if_capacity_less (newCapacity, newCapacity);
        }

        void resize (std::size_t newSize) {
            reserve (newSize);
        
            while (size() != newSize) {
                size() < newSize ? push_back (T{}) : pop_back();
            }
        }   

    private:
        struct InitTag{};
        struct MoveInitTag{};
        
        DynamicArray (std::size_t memSize, InitTag _)
            : capacity_ (memSize)
            , begin_ (mem_alloc (memSize))
            , end_ (begin_)
        {}

        DynamicArray (iterator beg, iterator end, std::size_t memSize, MoveInitTag _)
            : DynamicArray (memSize, InitTag{})
        {
            algs::move (beg, end, algs::back_inserter (*this));
        }

        T* mem_alloc (std::size_t count) {
            if (count == 0)
                return nullptr;

            return reinterpret_cast<T*> (
                ::operator new (sizeof(T) * count)
            );
        }

        void mem_free (T* ptr) noexcept {
            ::operator delete(ptr);
        }

        void realloc_if_capacity_less (std::size_t lowerBound, std::size_t newCapacity) {
            if (capacity() >= lowerBound)
                return;

            DynamicArray tmp (begin(), end(), newCapacity, MoveInitTag{});
            swap (tmp);
        }

        void reserve_before_insert () {
            auto memSize = empty() ? minCapacity : size() * 2;
            realloc_if_capacity_less (size() + 1, memSize);
        }

        static
        T* no_const (T const* ptr) noexcept {
            return const_cast<T*> (ptr);
        }

    private:
        static const
        std::size_t minCapacity = 100;

        std::size_t capacity_ = 0;

        T* begin_ = nullptr;
        T* end_ = nullptr;
    };


    template <typename T>
    void swap (DynamicArray<T>& lhs, DynamicArray<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif