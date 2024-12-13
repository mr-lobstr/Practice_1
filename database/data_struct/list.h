#ifndef MY_LIST_GUARD_H
#define MY_LIST_GUARD_H

#include <utility>
#include "iterators.h"
#include "my_algorithm.h"
#include "../utility/ptr_box.h"

namespace data_struct
{
    template <typename T>
    class List {
        struct Head;
        struct Node;
        class IterImpl;

        friend IterImpl;

    public:
        using value_type = T;

        using iterator       = iter::BidirectIterator<T, IterImpl, iter::Mutable_tag>;
        using const_iterator = iter::BidirectIterator<T, IterImpl, iter::Const_tag>;
    
    public:
        List() noexcept = default;

        List (List&& rhs) noexcept
          : size_ (rhs.size_)
          , endHead (rhs.endHead)
        {
            set_end_head (rhs.empty(), endHead);
            rhs.size_ = 0;
            rhs.endHead.reset();
        }

        List (List const& rhs) {
            algs::copy (rhs.begin(), rhs.end(), iter::back_inserter (*this));
        }

        template <class Iter, class = iter::EnableIfForward<Iter>>
        List (Iter beg, Iter end) {
            algs::copy (beg, end, iter::back_inserter (*this));
        }

        List (std::initializer_list<T> iList) {
            algs::copy (iList.begin(), iList.end(), iter::back_inserter (*this));
        }

        List (std::size_t count, T const& value = T()) {
            while (count--) {
                push_back (value);
            }
        }

        List& operator= (List&& rhs) noexcept {
            if (this != &rhs) {
                auto tmp {std::move (rhs)};
                swap (tmp);
            }
            return *this;
        }

        List& operator= (List const& rhs) {
            if (this != &rhs) {
                auto tmp {rhs};
                swap (tmp);
            }
            return *this;
        }

        ~List() noexcept {
            erase (begin(), end());
        }

        friend
        bool operator== (List const& lhs, List const& rhs) noexcept {
            return algs::equal (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
        }

        friend
        bool operator!= (List const& lhs, List const& rhs) noexcept {
            return not (lhs == rhs);
        }

        std::size_t size() const noexcept {
            return size_;
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        void swap (List& rhs) noexcept {
            std::swap (size_, rhs.size_);
            std::swap (endHead, rhs.endHead);

            set_end_head (empty(), endHead);
            set_end_head (rhs.empty(), rhs.endHead);
        }

        iterator begin() noexcept {
            return iterator {endHead.next};
        }

        auto cbegin() const noexcept {
            return const_iterator {endHead.next};
        }

        auto begin() const noexcept {
            return cbegin();
        }

        auto end() noexcept {
            return iterator {&endHead};
        }

        auto cend() const noexcept {
            return const_iterator {no_const (&endHead)};
        }

        auto end() const noexcept {
            return cend();
        }

        T& front() noexcept {
            return *begin();
        }

        T const& front() const noexcept {
            return *begin();
        }

        T& back() noexcept {
            return *--end();
        }

        T const& back() const noexcept {
            return *--end();
        }

        template <typename... Ts>
        iterator emplace (const_iterator it, Ts&&... params) {
            auto newNode = new Node (
                it.ptr->prev
              , it.ptr
              , std::forward<Ts> (params)...
            );

            newNode->bind();
            ++size_;

            return iterator {it.ptr->prev};
        }

        iterator insert (const_iterator it, T&& value) {
            return emplace (it, std::move (value));
        }

        iterator insert (const_iterator it, T const& value) {
            return emplace (it, value);
        }

        template <typename... Ts>
        void emplace_front (Ts&&... params) {
            emplace (begin(), std::forward<Ts> (params)...);
        }

        void push_front (T&& value) {
            emplace_front (std::move (value));
        }

        void push_front (T const& value) {
            emplace_front (value);
        }

        template <typename... Ts>
        void emplace_back (Ts&&... params) {
            emplace (end(), std::forward<Ts> (params)...);
        }

        void push_back (T&& value) {
            emplace_back (std::move (value));
        }

        void push_back (T const& value) {
            emplace_back (value);
        }

        iterator erase (const_iterator it) noexcept {            
            auto next = it.ptr->next;

            --size_;
            it.ptr->rebind();
            delete (get_ptr_node (it.ptr));

            return iterator {next};
        }

        iterator erase (const_iterator beg, const_iterator end) noexcept {
            while (beg != end) {
                beg = erase(beg);
            }
            
            return iterator {beg.ptr};
        }

        void pop_front() noexcept {
            erase (begin());
        }

        void pop_back() noexcept {
            erase (--end());
        }

    private:
        static
        void set_end_head (bool cond, Head& head) noexcept {
            (cond ? head.reset() : head.bind());
        }

        static
        Node* get_ptr_node (Head* pHead) noexcept {
            return static_cast<Node*> (pHead);
        }

        static
        Head* no_const (Head const* pHead) noexcept {
            return const_cast<Head*> (pHead);
        }
    
    private:
        Head endHead{};
        std::size_t size_ = 0;
    };


    template <typename T>
    struct List<T>::Head {
        void bind() noexcept {
            next->prev = prev->next = this;
        }

        void rebind() noexcept {
            prev->next = next;
            next->prev = prev;
        }

        void reset() noexcept {
            prev = next = this;
        }

        Head* prev = this;
        Head* next = this;
    };


    template <typename T>
    struct List<T>::Node:
        public Head
    {
        template <typename... Args>
        Node (Head* pPrev, Head* pNext, Args&&... args)
            : Head {pPrev, pNext}
            , value (std::forward<Args> (args)...)
        {}

        T value;
    };


    template <typename T>
    class List<T>::IterImpl
        : PtrBox<Head>
    {
        using PtrBox<Head>::ptr;
        friend List;

    public:
        IterImpl (Head* pHead = nullptr) noexcept
               : PtrBox<Head> (pHead)
        {}
    
    protected:
        bool equal(IterImpl const& rhs) const noexcept {
            return ptr == rhs.ptr;
        }

        void next() noexcept {
            ptr = ptr->next;
        }

        void prev() noexcept {
            ptr = ptr->prev;
        }
            
        T& get_value() const noexcept {
            return List::get_ptr_node (ptr)->value;
        }
    };


    template <typename T>
    void swap (List<T>& lhs, List<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif