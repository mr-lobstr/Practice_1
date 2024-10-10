#ifndef MY_HASH_TABLE_H_GUARD
#define MY_HASH_TABLE_H_GUARD

#include <stdexcept>
#include "hash_set.h"

namespace data_struct
{
    template <typename Key, typename Value>
    struct KeyValueHash {
        size_t operator() (Pair<Key, Value> const& kv) const {
            return Hasher<Key>{} (kv.first);
        }
    };

    template <typename Key, typename Value>
    struct KeyValueEqual {
        bool operator() (Pair<Key, Value> const& kv, Pair<Key, Value> const& kv2) const {
            return kv.first == kv2.first;
        }
    };


    template <
        typename Key
      , typename Value
      , typename Hash = KeyValueHash<Key, Value>
      , typename Eq = KeyValueEqual<Key, Value>
    >
    class HashTable {
        using Elem = Pair<Key, Value>;
        using Impl = HashSet<Elem, Hash, Eq>;
        using Self = HashTable;

    public:
        using iterator       = typename Impl::iterator;
        using const_iterator = typename Impl::const_iterator;

    public:
        HashTable() noexcept = default;
        ~HashTable() noexcept = default;

        HashTable(Self const&) = default;
        HashTable(Self&&) noexcept = default;

        Self& operator= (Self const&) = default;
        Self& operator= (Self&&) noexcept = default;

        HashTable (std::initializer_list<Elem> iList)
            : impl (iList)
        {}

        void swap (HashTable& rhs) noexcept {
            impl.swap(rhs.impl);
        }

        bool empty() const noexcept {
            return impl.empty();
        }

        std::size_t size() const noexcept {
            return impl.size();
        }

        auto cbegin() const noexcept {
            return impl.cbegin();
        }

        auto begin() const noexcept {
            return cbegin();
        }

        auto cend() const noexcept {
            return impl.cend();
        }

        auto end() const noexcept {
            return cend();
        }

        void add (Key const& key, Value const& value = Value{}) {
            Elem el {key, value};
            impl.add (el);
        }

        void erase (Key const& key) {
            impl.erase (
                Elem {key, Value{}}
            );
        }

        Value& operator[] (Key const& key) {
            return get (key);
        }

        Value const& operator[] (Key const& key) const {
            return get (key);
        }

        const_iterator find (Key const& key) const {
            return impl.find (Elem {key, Value{}});
        }

    private:
        Value& get (Key const& key) const {
            auto it = find (key);

            if (it == end()) throw std::runtime_error (
                "не существует элемента с таким ключом\n"
            );

            return const_cast<Value&> (it->second);
        }

    private:
        Impl impl{};
    };


    template <typename Key, typename Value, typename Hash>
    void swap (HashTable<Key, Value, Hash>& lhs, HashTable<Key, Value, Hash>& rhs) noexcept {
        lhs.swap (rhs);
    }
}

#endif