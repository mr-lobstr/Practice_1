#ifndef DYNAMIC_ARRAY_GUARD_H
#define DYNAMIC_ARRAY_GUARD_H

#include <stdexcept>
#include <utility>
#include "iterators.h"
#include "my_algorithm.h"
#include "../utility/utility.h"


namespace data_struct
{
    template <typename T>
    class DynamicArray {
        class IterImpl;
        friend IterImpl;

    public:
        using value_type = T;

        using iterator       = iter::RandomIterator<T, IterImpl, iter::Mutable_tag>;
        using const_iterator = iter::RandomIterator<T, IterImpl, iter::Const_tag>;

    public:
        DynamicArray() noexcept = default;

        DynamicArray (DynamicArray&& rhs) noexcept
            : capacity_ (std::exchange (rhs.capacity_, 0))
            , begin_ (std::exchange (rhs.begin_, nullptr))
            , end_ (std::exchange (rhs.end_, nullptr))
        {}

        template <class Iter, class = iter::EnableIfForward<Iter>>
        DynamicArray (Iter beg, Iter end)
            : DynamicArray ()
        {
            algs::copy (beg, end, iter::back_inserter (*this));
        }

        DynamicArray (std::initializer_list<T> iList)
            : DynamicArray (iList.size(), InitTag{})
        {
            algs::copy (iList.begin(), iList.end(), iter::back_inserter (*this));
        }

        DynamicArray (DynamicArray const& rhs)
            : DynamicArray (rhs.size(), InitTag{})
        {
            algs::copy (rhs.begin(), rhs.end(), iter::back_inserter (*this));
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
                auto tmp (std::move (rhs));
                swap (tmp);
            }
            return *this;
        }

        DynamicArray& operator= (DynamicArray const& rhs) {
            if (this != &rhs) {
                auto tmp (rhs);
                swap (tmp);
            }
            return *this;
        }

        ~DynamicArray() noexcept {
            resize(0);
            mem_free (begin_);
        }


        friend
        bool operator== (DynamicArray const& lhs, DynamicArray const& rhs) noexcept {
            return algs::equal (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }

        friend
        bool operator!= (DynamicArray const& lhs, DynamicArray const& rhs) noexcept {
            return not (lhs == rhs);
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

            new(end_) T (std::forward<Ts> (args)...);
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
            new (saveIt.ptr) T (std::forward<Ts> (args)...);

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
            algs::shift_left (it.ptr, end().ptr);
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
            algs::move (beg, end, iter::back_inserter (*this));
        }

        T* mem_alloc (std::size_t count) {
            if (count == 0)
                return nullptr;

            return reinterpret_cast<T*> (
                ::operator new (sizeof(T) * count)
            );
        }

        void mem_free (T* ptr) noexcept {
            ::operator delete (ptr);
        }

        void realloc_if_capacity_less (std::size_t lowerBound, std::size_t newCapacity) {
            if (capacity() >= lowerBound)
                return;

            DynamicArray tmp (begin(), end(), newCapacity, MoveInitTag{});
            swap (tmp);
        }

        void reserve_before_insert () {
            auto memSize = empty() ? 1 : size() * 2;
            realloc_if_capacity_less (size() + 1, memSize);
        }

        static
        T* no_const (T const* ptr) noexcept {
            return const_cast<T*> (ptr);
        }

    private:
        std::size_t capacity_ = 0;

        T* begin_ = nullptr;
        T* end_ = nullptr;
    };


    template <typename T>
    void swap (DynamicArray<T>& lhs, DynamicArray<T>& rhs)
    {
        lhs.swap(rhs);
    }


    template <typename T>
    class DynamicArray<T>::IterImpl
        : PtrBox<T>
    {
        using PtrBox<T>::ptr;
        friend DynamicArray;

    public:
        IterImpl (T* ptr_ = nullptr) noexcept
               : PtrBox<T> (ptr_)
        {}
    
    protected:
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
            return const_cast<T&> (*ptr); 
        }
    };

}

#endif