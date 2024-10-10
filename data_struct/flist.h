#ifndef MY_FORWARD_LIST_GUARD_H
#define MY_FORWARD_LIST_GUARD_H

#include <utility>
#include "iterators.h"
#include "my_algorithm.h"


namespace flist_detail
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
            : pPrev (std::exchange (rhs.pPrev, nullptr))
        {}

        IterImpl& operator= (IterImpl&& rhs) noexcept {
            pPrev = std::exchange (rhs.pPrev, nullptr);
            return *this;
        }

        IterImpl (Head* pH) noexcept
            : pPrev (pH)
        {}

    public:
        Head* real() const noexcept {
            return pPrev;
        }

        bool equal(IterImpl const& rhs) const noexcept {
            return pPrev == rhs.pPrev;
        }

        void next() noexcept {
            pPrev = pPrev->next;
        }
            
        T& get_value() const noexcept {
            return C::get_ptr_node (pPrev)->value;
        }

    public:
        Head* pPrev = nullptr;
    };
}


namespace data_struct
{
    template <typename T>
    class FList {
        struct Head {
            Head* next = nullptr;
        };

        struct Node: public Head {
            T value;
        };

        using IterImpl = flist_detail::IterImpl<T, FList>;
        friend IterImpl;
        friend InserterIterator<T, FList>;

    public:
        using iterator       = ForwardIterator<T, IterImpl, Mutable_tag>;
        using const_iterator = ForwardIterator<T, IterImpl, Const_tag>;

    public:
        FList() noexcept = default;

        FList (FList&& rhs) noexcept
            : prevFirst (std::exchange (rhs.prevFirst, Head{}))
        {}

        FList (FList const& rhs) {
            algs::copy (rhs.begin(), rhs.end(), algs::inserter (*this, prev_begin()));
        }

        template <class Iter, class = EnableIfForward<Iter>>
        FList (Iter beg, Iter end) {
            algs::copy (beg, end, algs::inserter (*this, prev_begin()));
        }

        FList (std::initializer_list<T> iList) {
            algs::copy (iList.begin(), iList.end(), algs::inserter (*this, prev_begin()));
        }

        FList (std::size_t count, T const& value = T()) {
            while (count--) {
                push_front (value);
            }
        }

        FList& operator= (FList&& rhs) noexcept {
            if (this != &rhs) {
                auto tmp = std::move (rhs);
                swap (tmp);
            } 
            return *this;
        }

        FList& operator= (FList const& rhs) {
            if (this != &rhs) {
                auto tmp = rhs;
                swap (tmp);
            }
            return *this;
        }

        ~FList() noexcept {
            while (not empty()) {
                pop_front();
            }
        }

        bool empty() const noexcept {
            return prevFirst.next == nullptr;
        }

        void swap (FList& rhs) noexcept {
            std::swap(prevFirst, rhs.prevFirst);
        }
        
        auto prev_begin() noexcept {
            return iterator {&prevFirst};
        }

        auto prev_cbegin() const noexcept {
            return const_iterator {no_const (&prevFirst)};
        }
        auto prev_begin() const noexcept {
            return prev_cbegin();
        }

        auto begin() noexcept {
            return iterator {prevFirst.next};
        }

        auto cbegin() const noexcept {
            return const_iterator {prevFirst.next};
        }

        auto begin() const noexcept {
            return cbegin();
        }

        auto end() noexcept {
            return iterator {nullptr};
        }

        auto cend() const noexcept {
            return const_iterator {nullptr};
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

        template <typename... Ts>
        iterator emplace_after (const_iterator it, Ts&&... params) {
            auto pPrev = it.real();
            auto oldNext = pPrev->next;

            pPrev->next = new Node {
                oldNext
              , std::forward<Ts> (params)...
            };

            return iterator {it.real()};
        }

        iterator insert_after (const_iterator it, T const& value) {
            return emplace_after (it, value);
        }

        iterator insert_after (const_iterator it, T&& value) {
            return emplace_after (it, std::move (value));
        }

        template <typename... Ts>
        void emplace_front (Ts&&... params) {
            emplace_after (prev_begin(), std::forward<Ts> (params)...);
        }

        void push_front (T&& value) {
            emplace_front (std::move (value));
        }

        void push_front (T const& value) {
            emplace_front (value);
        }

        void erase_after (const_iterator it) noexcept {
            auto pPrev = it.real();
            auto oldFirst = get_ptr_node (pPrev->next);
            pPrev->next = oldFirst->next;

            delete (oldFirst);
        }

        void pop_front() noexcept {
            erase_after (prev_begin());
        }
        
        template <typename Predicate>
        iterator find_prev_if (Predicate pred) noexcept {
            return find_previous_if (pred);
        }

        template <typename Predicate>
        const_iterator find_prev_if (Predicate pred) const noexcept {
            return find_previous_if (pred);
        }  

        iterator find_prev (T const& value) noexcept {
            return find_prev_if ([&] (auto& el) {
                return value == el;
            });
        }

        const_iterator find_prev (T const& value) const noexcept {
            return find_prev_if ([&] (auto& el) {
                return value == el;
            });
        }       

    private:
        static
        Head* no_const (Head const* pHead) noexcept {
            return const_cast<Head*> (pHead);
        }

        static
        Node* get_ptr_node (Head* pHead) noexcept {
            return static_cast<Node*> (pHead);
        }

        template <typename Predicate>
        iterator find_previous_if (Predicate pred) const noexcept {
            iterator it {no_const (&prevFirst)};

            while (next_iter(it) != end()) {
                if (pred (*next_iter(it)))
                    break;
                ++it;
            }

            return it;
        }

        // только для Inserter
        iterator insert (const_iterator it, T const& value) {
            return insert_after (it, value);
        }

        iterator insert (const_iterator it, T&& value) {
            return insert_after (it, std::move (value));
        }
    
    private:
        Head prevFirst{};
    };


    template <typename T>
    void swap (FList<T>& lhs, FList<T>& rhs) noexcept {
        lhs.swap (rhs);
    }
}
#endif