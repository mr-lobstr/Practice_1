#ifndef MY_LIST_GUARD_H
#define MY_LIST_GUARD_H

#include <utility>
#include "iterators.h"
#include "my_algorithm.h"


namespace list_detail
{
    template <typename T, typename C>
    struct IterImpl {
        using Container = C;
        using Head = typename C::Head;

    public:
        IterImpl() noexcept = default;
        ~IterImpl() noexcept = default;
        IterImpl (IterImpl const&) noexcept = default;
        IterImpl& operator= (IterImpl const&) noexcept = default;

        IterImpl (IterImpl&& rhs) noexcept
            : pHead (std::exchange (rhs.pHead, nullptr))
        {}

        IterImpl& operator= (IterImpl&& rhs) noexcept {
            pHead = std::exchange (rhs.pHead, nullptr);
            return *this;
        }

        IterImpl (Head* pH) noexcept
            : pHead (pH)
        {}

    public:
        Head* real() const noexcept {
            return pHead;
        }

        bool equal(IterImpl const& rhs) const noexcept {
            return pHead == rhs.pHead;
        }

        void next() noexcept {
            pHead = pHead->next;
        }

        void prev() noexcept {
            pHead = pHead->prev;
        }
            
        T& get_value() const noexcept {
            return C::get_ptr_node (pHead)->value;
        }

    public:
        Head* pHead = nullptr;
    };
}


namespace data_struct
{
    template <typename T>
    class List {
        struct Head {
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

        struct Node: public Head {
            T value;
        };

        using IterImpl = list_detail::IterImpl<T, List>;
        friend IterImpl;

    public:
        using iterator       = BidirectionalIterator<T, IterImpl, Mutable_tag>;
        using const_iterator = BidirectionalIterator<T, IterImpl, Const_tag>;
    
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
            algs::copy (rhs.begin(), rhs.end(), algs::back_inserter (*this));
        }

        template <class Iter, class = EnableIfForward<Iter>>
        List (Iter beg, Iter end) {
            algs::copy (beg, end, algs::back_inserter (*this));
        }

        List (std::initializer_list<T> iList) {
            algs::copy (iList.begin(), iList.end(), algs::back_inserter (*this));
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
            auto pHead = it.real();
            auto newNode = new Node {
                Head {pHead->prev, pHead}
              , T (std::forward<Ts> (params)...)
            };
            newNode->bind();
            ++size_;

            return iterator {it.real()->prev};
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
            auto pHead = it.real();
            auto pNext = pHead->next;

            --size_;
            pHead->rebind();
            delete (get_ptr_node (pHead));

            return iterator {pNext};
        }

        iterator erase (const_iterator beg, const_iterator end) noexcept {
            while (beg != end) {
                beg = erase(beg);
            }
            
            return iterator {beg.real()};
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
    void swap (List<T>& lhs, List<T>& rhs)
    {
        lhs.swap(rhs);
    }
}

#endif