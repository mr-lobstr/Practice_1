#ifndef MY_FORWARD_LIST_GUARD_H
#define MY_FORWARD_LIST_GUARD_H

#include <utility>
#include "iterators.h"
#include "my_algorithm.h"
#include "my_utility.h"

namespace data_struct
{
    template <typename T>
    class FList {
        struct Head;
        struct Node;
        class IterImpl;

        friend IterImpl;

    public:
        using value_type = T;

        using iterator       = iter::ForwardIterator<T, IterImpl, iter::Mutable_tag>;
        using const_iterator = iter::ForwardIterator<T, IterImpl, iter::Const_tag>;

    public:
        FList() noexcept = default;

        FList (FList&& rhs) noexcept
            : prevFirst (std::exchange (rhs.prevFirst, Head{}))
        {}

        FList (FList const& rhs) {
            algs::copy (rhs.begin(), rhs.end(), iter::inserter (*this, prev_begin()));
        }

        template <class Iter, class = iter::EnableIfForward<Iter>>
        FList (Iter beg, Iter end) {
            algs::copy (beg, end, iter::inserter (*this, prev_begin()));
        }

        FList (std::initializer_list<T> iList) {
            algs::copy (iList.begin(), iList.end(), iter::inserter (*this, prev_begin()));
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
            return const_iterator {no_const_head  (&prevFirst)};
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
            return iterator{};
        }

        auto cend() const noexcept {
            return const_iterator{};
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
            auto oldNext = it.ptr->next;

            it.ptr->next = new Node (
                oldNext
              , std::forward<Ts> (params)...
            );

            return iterator {it.ptr};
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
            auto oldNext = get_ptr_node (it.ptr->next);
            it.ptr->next = oldNext->next;

            delete (oldNext);
        }

        void pop_front() noexcept {
            erase_after (prev_begin());
        }

        template <typename Predicate>
        auto find_prev_if (Predicate pred) const noexcept {
            auto it = prev_begin();

            while (next_iter(it) != end()) {
                if (pred (*next_iter(it)))
                    break;
                ++it;
            }

            return it;
        }

        auto find_prev (T const& value) const noexcept {
            return find_prev_if ([&] (auto& el) {
                return value == el;
            });
        } 
        
        template <typename Predicate>
        auto find_prev_if (Predicate pred) noexcept {
            return iterator {
                const_this().find_prev_if (pred).ptr
            };
        }   

        auto find_prev (T const& value) noexcept {
            return iterator {
                const_this().find_prev (value).ptr
            };
        }

        void insert_after_node (const_iterator prevPos, const_iterator prevNode) noexcept {
            auto pNode = prevNode.ptr->next;
            prevNode.ptr->next = pNode->next;

            pNode->next = prevPos.ptr->next;
            prevPos.ptr->next = pNode;
        }

        void push_front_node (const_iterator prevNode) noexcept {
            insert_after_node (prev_begin(), prevNode);
        }

    private:
        FList const& const_this() const noexcept {
            return *this;
        }

        static
        Head* no_const_head (Head const* pHead) noexcept {
            return const_cast<Head*> (pHead);
        }

        static
        Node* get_ptr_node (Head* pHead) noexcept {
            return static_cast<Node*> (pHead);
        }
    
    private:
        Head prevFirst{};
    };


    template <typename T>
    struct FList<T>::Head {
        Head* next = nullptr;
    };


    template <typename T>
    struct FList<T>::Node:
        public Head
    {
        template <typename... Args>
        Node (Head* pHead, Args&&... args)
            : Head {pHead}
            , value (std::forward<Args> (args)...)
        {}

        T value;
    };

    
    template <typename T>
    class FList<T>::IterImpl
        : PtrBox<Head>
    {
        using PtrBox<Head>::ptr;
        friend FList;

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
            
        T& get_value() const noexcept {
            return FList::get_ptr_node (ptr)->value;
        }
    };


    template <typename T>
    void swap (FList<T>& lhs, FList<T>& rhs) noexcept {
        lhs.swap (rhs);
    }
}


namespace iter
{
    template <typename T>
    struct InserterImpl<data_struct::FList<T>> {
        using FList = data_struct::FList<T>;

        using OutType = InserterImpl&;
        using Value = typename FList::value_type;
        using CIter = typename FList::const_iterator;

    public:
        InserterImpl (FList& flist_, CIter pos) noexcept
            : flist (flist_)
            , insertPos (pos)
        {}

        void operator= (Value const& value) {
            insertPos = flist.insert_after (insertPos, value);
            ++insertPos;
        }

        void operator= (Value&& value) {
            insertPos = flist.insert_after (insertPos, std::move (value));
            ++insertPos;
        }

        OutType operator*() noexcept {
            return *this;
        }

    private:
        FList& flist;
        CIter insertPos{};
    };
}

#endif