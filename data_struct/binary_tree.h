#ifndef FBINARY_TREE_H_GUARD
#define FBINARY_TREE_H_GUARD

#include "stack.h"
#include "iterators/iterators_general.h"

namespace data_struct
{
    enum class Tilt {
        to_left
      , to_right
    };


    template <typename T>
    class BinTree {
        struct Node {
            Node* left;
            Node* right;
            T value;
        };

        template <typename Mut>
        class BranchIterator {
            using Self = BranchIterator;
            using Ptr = Node*;
            using ReturnType = std::conditional_t<
                std::is_same_v<Mut, Mutable_tag>
              , T
              , T const
            >;

            template <typename Mut_>
            using EnableIfMutable = std::enable_if_t<std::is_same_v<Mut_, Mutable_tag>>;

            friend BinTree;
            friend BranchIterator<Const_tag>;
       
        public:
            BranchIterator() noexcept = default;
            BranchIterator (Self const&) noexcept = default;
            Self& operator= (Self const&) noexcept = default;
            ~BranchIterator() noexcept = default;

            BranchIterator (Self&& rhs) noexcept
                : ptrPtr (std::exchange (rhs.ptrPtr, nullptr))
            {}

            Self& operator= (Self&& rhs) noexcept {
                ptrPtr = std::exchange (rhs.ptrPtr, nullptr);
                return *this;
            }

            BranchIterator (Ptr* ptr) noexcept
                : ptrPtr (ptr)
            {}

            template <typename Mut_, typename = EnableIfMutable<Mut_>>
            BranchIterator (BranchIterator<Mut_> const& it) noexcept
                : ptrPtr (it.ptrPtr)
            {}

            friend
            bool operator== (Self const& lhs, Self const& rhs) noexcept {
                if (lhs.is_end() or rhs.is_end())
                    return lhs.is_end() and rhs.is_end();

                return lhs.ptrPtr == rhs.ptrPtr;
            }

            friend
            bool operator!= (Self const& lhs, Self const& rhs) noexcept {
                return not (lhs == rhs);
            }

            ReturnType& operator*() const noexcept {
                return ptr_node()->value;
            }

            ReturnType* operator->() const noexcept {
                return &operator*();
            }

            Self left() const noexcept {
                auto tmp = *this;
                tmp.to_left();
                return tmp;
            }

            Self right() const noexcept {
                auto tmp = *this;
                tmp.to_right();
                return tmp;
            }

            void to_left() noexcept {
                ptrPtr = & ptr_node()->left;
            }

            void to_right() noexcept {
                ptrPtr = & ptr_node()->right;
            }

            bool is_leaf() const noexcept {
                return (not is_end()) 
                   and ptr_node()->left == nullptr
                   and ptr_node()->right == nullptr;
            }

            bool is_end() const noexcept {
                return ptrPtr == nullptr or ptr_node() == nullptr;
            }

        private:
            Ptr& ptr_node() const noexcept {
                return *ptrPtr;
            }
    
        private:
            Ptr* ptrPtr = nullptr;
        };
    
    public:
        using BranchIt = BranchIterator<Mutable_tag>;
        using ConstBranchIt = BranchIterator<Const_tag>;
    
    public:
        BinTree() noexcept = default;

        BinTree (BinTree&& rhs) noexcept
            : ptrRoot (std::exchange (rhs.ptrRoot, nullptr))
        {}

        BinTree (BinTree const& rhs) noexcept
        {
            if (rhs.empty())
                return;
            
            Stack<ConstBranchIt> stack{};
            stack.push (rhs.root());
            auto it = root();

            while (not stack.empty()) {
                auto& top = stack.top();

                if (top.is_leaf()) {
                    insert (it, *top);
                    stack.pop();
                } else if (top.left().is_end()) {
                    insert (it.right(), T{});
                    stack.push (top.right());                    
                } else {

                    insert (it.left(), T{});
                    stack.push (top.left());
                }
            }
        }

        BinTree& operator= (BinTree&& rhs) noexcept {
            if (this != &rhs) {
                auto tmp {std::move (rhs)};
            }
            return *this;
        }

        BinTree& operator= (BinTree const& rhs) noexcept {
            if (this != &rhs) {
                auto tmp {std::move (rhs)};
            }
            return *this;
        }

        ~BinTree() noexcept {
            if (empty())
                return;
            
            try {
                clear();
            } catch (...) {
                // std::terminate();
            }
        };


        bool empty() const noexcept {
            return root().is_end();
        }

        auto root() noexcept {
            return BranchIt (&ptrRoot);
        }

        auto croot() const noexcept {
            return ConstBranchIt (
                const_cast<Node**> (&ptrRoot)
            );
        }

        auto root() const noexcept {
            return croot();
        }

        template <typename... Ts>
        void emplace (Tilt tilt, ConstBranchIt it, Ts&&... params) {
            auto ptrLeft = (tilt == Tilt::to_left) ? it.ptr_node() : nullptr;
            auto ptrRight = (tilt == Tilt::to_right) ? it.ptr_node() : nullptr;

            it.ptr_node() = new Node {
                ptrLeft
              , ptrRight
              , std::forward<Ts> (params)...
            };
        }

        void insert (Tilt tilt, ConstBranchIt it, T&& value) {
            emplace (tilt, it, std::move (value));
        }

        void insert (Tilt tilt, ConstBranchIt it, T const& value) {
            emplace (tilt, it, value);
        }

        template <typename... Ts>
        void emplace_root (Tilt tilt, Ts&&... params) {
            emplace (tilt, root(), std::forward<Ts> (params)...);
        }

        void push_root (Tilt tilt, T&& value) {
            emplace_root (tilt, std::move (value));
        }

        void push_root (Tilt tilt, T const& value) {
            emplace_root (tilt, value);
        }

        void erase_leaf (ConstBranchIt it) {
            if (not it.is_leaf()) throw std::runtime_error (
                "данный узел не является листом\n"
            );

            delete (it.ptr_node());
            it.ptr_node() = nullptr;
        }

        void add_subtree (ConstBranchIt it, ConstBranchIt subTreeRoot) {
            if (not it.is_end()) throw std::runtime_error (
                "данный узел не является конечным\n"
            );

            std::swap (it.ptr_node(), subTreeRoot.ptr_node());
        }

        void clear() {
            if (empty())
                return;
            
            Stack<ConstBranchIt> stack{};
            stack.push (root());

            while (not stack.empty()) {
                auto& top = stack.top();

                if (top.is_leaf()) {
                    erase_leaf (top);
                    stack.pop();
                } else if (top.left().is_end()) {
                    stack.push (top.right());
                } else {
                    stack.push (top.left());
                }
            }
        }
    
    private:
        Node* ptrRoot = nullptr;
    };
}

#endif