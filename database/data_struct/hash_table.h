#ifndef MY_HASH_TABLE_H_GUARD
#define MY_HASH_TABLE_H_GUARD

#include <stdexcept>
#include "hash_container_base.h"
#include "default_hesh.h"
#include "iterators.h"
// #include "../my_utility/my_utility.h"


namespace data_struct
{
    template <typename Key, typename Value>
    struct KeyValue {
        KeyValue() = default;

        KeyValue (Pair<Key, Value> kv)
            : key (kv.first)
            , value (kv.second)
        {}

        template <
            typename KEY
          , typename = std::enable_if_t<
                std::is_constructible<Key, KEY>::value
            >
          , typename... Args
        >
        KeyValue (KEY&& k, Args&&... args)
            : key (std::forward<KEY> (k))
            , value (std::forward<Args> (args)...)
        {}

        Key key{};
        Value value{};
    };


    template <typename Key, typename Value, typename Hash = Hasher<Key>>
    class HashTable
        : private HashContainerBase<
            KeyValue<Key, Value>
          , Hash
          , HashTable<Key, Value, Hash>
        >
    {
        using KV = KeyValue<Key, Value>;
        using Base = HashContainerBase<KV, Hash, HashTable>;
        friend Base;

    public:
        using value_type = KV;

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
        using Base::rehash_before_add;
        using Base::equal;

        using Base::hash;
        using Base::buckets;
        using Base::size_;

    public:
        HashTable() noexcept = default;

        template <class Iter, class = iter::EnableIfForward<Iter>>
        HashTable (Iter beg, Iter end) {
            algs::copy (beg, end, iter::add_iterator (*this));
        }

        HashTable (std::initializer_list<KV> initList) 
            : HashTable (initList.begin(), initList.end())
        {}

        friend
        bool operator== (HashTable const& lhs, HashTable const& rhs) noexcept {
            return lhs.equal (rhs);
        }

        friend
        bool operator!= (HashTable const& lhs, HashTable const& rhs) noexcept {
            return not (lhs == rhs);
        }


        template <
            typename KEY
          , typename = std::enable_if_t<
                std::is_constructible<Key, KEY>::value
            >
          , typename... Args
        >
        iterator emplace_add (KEY&& key, Args&&... args) {
            auto it = find (key);

            if (it == end()) {
                if (rehash_before_add()) {
                    it = find (key);
                }

                it.bucketIt->emplace_after (
                    it.prevElemIt
                  , std::forward<KEY> (key)
                  , std::forward<Args> (args)...
                );
    
                ++size_;
            }

            return it;
        }

        iterator add (KV const& kv) {
            return emplace_add (kv.key, kv.value);
        }

        iterator add (KV&& kv) {
            return emplace_add (
                std::move (kv.key)
              , std::move (kv.value)
            );
        }

        template <typename... Args>
        iterator add (Key const& key, Args&&... args) {
            return emplace_add (
                key
              , std::forward<Args> (args)...
            );
        }

        void erase (Key const& key) noexcept {
            if (auto it = find (key); it != end()) {
                it.bucketIt->erase_after (it.prevElemIt);
                --size_;
            }
        }

        auto find (Key const& key) const noexcept {
            if (buckets.empty())
                return end();

            const_iterator it;
            it.bucketIt = find_bucket (key);
            it.endIt = buckets.end();

            it.prevElemIt = it.bucketIt->find_prev_if ([&] (auto& pair) {
                return key == pair.key;
            });

            return it;
        }

        auto find (Key const& key) noexcept {
            auto it = static_cast<HashTable const*> (this)->find (key);
            return iterator (
                it.bucketIt
              , it.endIt
              , it.prevElemIt
            );
        }

        Value const& operator[] (Key const& key) const {
            auto it = find (key);

            if (it == end()) throw std::invalid_argument (
                "не существует элемента с таким ключом: "
            );

            return it->value;
        }

        Value& operator[] (Key const& key) {
            auto const& cref = *this;
            return const_cast<Value&> (cref[key]);            
        }
    
    private:
        auto find_bucket (Key const& key) const noexcept {
            std::size_t index = hash (key) % buckets.size();
            return buckets.begin() + index;
        }

        auto find_element (KV const& kv) noexcept {
            return find (kv.key);
        }
    };
}

#endif