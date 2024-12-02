#ifndef MY_HASH_SET_H_GUARD
#define MY_HASH_SET_H_GUARD

#include <stdexcept>
#include "hash_container_base.h"
#include "default_hesh.h"
#include "iterators.h"


namespace data_struct
{
    template <typename T, typename Hash = Hasher<T>>
    class HashSet
        : private HashContainerBase<
            T
          , Hash
          , HashSet<T, Hash>
        >
    {
        using Base = HashContainerBase<T, Hash, HashSet>;
        friend Base;

    public:
        using value_type = T;

        using typename Base::iterator;
        using typename Base::const_iterator;
    
    public:
        using Base::empty;
        using Base::size;
        using Base::begin;
        using Base::cbegin;
        using Base::end;
        using Base::cend;
    
    private:
        using typename Base::Bucket;
        using Base::rehash_before_add;
        using Base::equal;

        using Base::hash;
        using Base::buckets;
        using Base::size_;

    public:
        HashSet() noexcept = default;

        template <class Iter, class = iter::EnableIfForward<Iter>>
        HashSet (Iter beg, Iter end) {
            algs::copy (beg, end, iter::add_iterator (*this));
        }

        HashSet (std::initializer_list<T> initList) 
            : HashSet (initList.begin(), initList.end())
        {}

        friend
        bool operator== (HashSet const& lhs, HashSet const& rhs) noexcept {
            return lhs.equal (rhs);
        }

        friend
        bool operator!= (HashSet const& lhs, HashSet const& rhs) noexcept {
            return not (lhs == rhs);
        }

        template <typename... Args>
        iterator emplace_add (Args&&... args) {
            Bucket tmp;
            tmp.emplace_front (std::forward<Args> (args)...);

            auto it = find (tmp.front());

            if (it == end()) {
                if (rehash_before_add()) {
                    it = find (tmp.front());
                }

                it.bucketIt->insert_after_node (
                    it.prevElemIt
                  , tmp.prev_begin()
                );
                
                ++size_;
            }

            return it;
        }

        iterator add (T const& value) {
            return emplace_add (value);
        }

        iterator add (T&& value) {
            return emplace_add (std::move (value));
        }

        void erase (T const& value) noexcept {
            if (auto it = find (value); it != end()) {
                it.bucketIt->erase_after (it.prevElemIt);
                --size_;
            }
        }

        auto find (T const& value) const noexcept {
            if (buckets.empty())
                return end();

            const_iterator it;
            it.bucketIt = find_bucket (value);
            it.endIt = buckets.end();

            it.prevElemIt = it.bucketIt->find_prev_if ([&] (auto& elem) {
                return value == elem;
            });

            return it;
        }

        auto find (T const& value) noexcept {
            auto it = static_cast<HashSet const*> (this)->find (value);
            return iterator (
                it.bucketIt
              , it.endIt
              , it.prevElemIt
            );
        }

    private:
        auto find_bucket (T const& value) const noexcept {
            std::size_t index = hash (value) % buckets.size();
            return buckets.begin() + index;
        }

        auto find_element (T const& value) noexcept {
            return find (value);
        }
    };
}

#endif