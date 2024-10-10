#ifndef MY_HASH_SET_H_GUARD
#define MY_HASH_SET_H_GUARD

#include "dynamic_array.h"
#include "flist.h"


namespace hashset_detail
{
    template <typename T, typename C>
    struct IterImpl {
        using Container = C;
        using buckets_iterator = typename C::buckets_iterator;
        using elements_iterator = typename C::elements_iterator;


    public:
        auto real() const noexcept {
            return algs::make_pair (bucketIt, prevElemIt);
        }

        bool equal (IterImpl const& rhs) const noexcept {
            if (is_end() or rhs.is_end())
                return is_end() and rhs.is_end();
                
            return prevElemIt == rhs.prevElemIt;
        }

        void next() noexcept {               
            ++prevElemIt;

            if (not is_end()) 
                return;
                
            while (++bucketIt != endIt and bucketIt->empty())
                ;

            if (bucketIt != endIt) {
                prevElemIt = bucketIt->prev_begin();
            }           
        }

        auto& get_value() const noexcept {
            return *elem_it();
        }
        
    private:
        auto elem_it() const noexcept {
            return next_iter(prevElemIt);
        }

        bool is_end() const noexcept {
            return bucketIt == endIt
                or elem_it() == bucketIt->end();
        }

    public:
        buckets_iterator  bucketIt{};
        buckets_iterator  endIt{};
        elements_iterator prevElemIt{};
    };
} 


namespace data_struct
{
    template <typename T>
    struct Hasher;


    template <typename T>
    struct DefaultEqual {
        bool operator() (T const& t1, T const& t2) const {
            return t1 == t2;
        }
    };


    template <typename T, typename Hash = Hasher<T>, typename Eq = DefaultEqual<T>>
    class HashSet {
        using Bucket = FList<T>;
        using Array  = DynamicArray<Bucket>;

        using buckets_iterator  = typename Array::iterator;
        using elements_iterator = typename Bucket::iterator;

        using IterImpl = hashset_detail::IterImpl<T, HashSet>;
        using BackIns = BackInserterIterator<T, HashSet>;

        friend IterImpl;
        friend BackIns;


        template <typename T1>
        using EnableIfIsT = std::enable_if_t<
            std::is_same_v<std::remove_reference<T1>, T>
         or std::is_constructible_v<T, T1>
        >;

    public:
        using iterator       = ForwardIterator<T, IterImpl, Mutable_tag>;
        using const_iterator = ForwardIterator<T, IterImpl, Const_tag>;

    public:
        HashSet() noexcept = default;
        ~HashSet() noexcept = default;

        HashSet (HashSet&& rhs) noexcept
            : array (std::move (rhs.array))
            , size_ (std::exchange (rhs.size_, 0))
        {}

        HashSet (HashSet const& rhs) noexcept
            : array (rhs.array)
            , size_ (rhs.size_)
        {}

        template <class Iter, class = EnableIfForward<Iter>>
        HashSet (Iter beg, Iter end) {
            algs::copy (beg, end, algs::back_inserter (*this));
        }

        HashSet (std::initializer_list<T> iList) {
            algs::copy (iList.begin(), iList.end(), BackIns (*this));
        }

        HashSet& operator= (HashSet&& rhs)
        {
            if (this == &rhs) {
                auto tmp {std::move (rhs)};
                swap (tmp);
            }
            return *this;
        }

        HashSet& operator= (HashSet const& rhs)
        {
            if (this == &rhs) {
                auto tmp {rhs};
                swap (tmp);
            }
            return *this;
        }
       
        void swap (HashSet& rhs) noexcept {
            using std::swap;

            swap (array, rhs.array);
            swap (size_, rhs.size_);
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        std::size_t size() const noexcept {
            return size_;
        }

        iterator begin() noexcept {
            return begin_iter_impl();
        }

        const_iterator cbegin() const noexcept {
            return begin_iter_impl();
        }

        const_iterator begin() const noexcept {
            return cbegin();
        }

        iterator end() noexcept {
            return IterImpl {
                array.end(), array.end(), elements_iterator{}
            };
        }

        const_iterator cend() const noexcept {
            auto [beg, end] = begin_end();
            return IterImpl {end, end, elements_iterator{}};
        }

        const_iterator end() const noexcept {
            return cend();
        }

        void erase (T const& value) noexcept {
            if (auto it = find (value); it != end()) {
                auto [bucketIt, prevIt] = it.real();

                bucketIt->erase_after (prevIt);
                --size_;
            }
        }

        template <class T1, class = EnableIfIsT<T1>>
        void add (T1&& value) {
            if (size() >= buckets_cnt() * middleMaxDepth) {
                refill();
            }

            if (auto pos = find (value); pos == end()) {
                push_to_bucket (std::forward<T1> (value));
            }
        }

        iterator find (T const& value) noexcept {
            return find_ (value);
        }

        const_iterator find (T const& value) const noexcept {
            return find_ (value);
        };

    private:
        std::size_t buckets_cnt() const noexcept {
            return array.size();
        }

        std::size_t bucket_number (T const& value) const noexcept {
            return Hash{} (value) % buckets_cnt();
        }

        auto begin_end() const noexcept {
            auto& arr = const_cast<Array&> (array);
            return algs::make_pair (arr.begin(), arr.end());
        }

        auto begin_iter_impl () const noexcept {
            auto [beg, end] = begin_end();

            beg = algs::find_if (beg, end, [] (auto& bucket) {
                return not bucket.empty();
            });

            return IterImpl {
                beg, end, (beg != end ? beg->prev_begin() : elements_iterator{})
            };
        }

        auto find_ (T const& value) const noexcept {
            auto [bucketIt, endIt] = begin_end();

            if (empty())
                return IterImpl {endIt, endIt, elements_iterator{}};

            bucketIt += bucket_number (value);
            auto prevElemIt = bucketIt->find_prev_if ([&] (auto& el) {
                return Eq{} (value, el);
            });

            return IterImpl {bucketIt, endIt, prevElemIt};
        }

        template <typename T1>
        void push_to_bucket (T1&& value) {
            auto bucketIt = array.begin() + bucket_number (value);
            bucketIt->push_front (std::forward<T1> (value));
            ++size_;
        }

        void refill() {
            if (empty()) {
                array.resize (minBucketCnt);
                return;
            }
            
            HashSet newSet;
            newSet.array.resize (buckets_cnt() * 1.5);

            algs::for_each (begin(), end(), [&] (auto& el) {
                newSet.push_to_bucket (el);
            });

            swap (newSet);
        }

        void push_back (T const& value) {
            add (value);
        }

        void push_back (T&& value) {
            add (std::move (value));
        }

    private:
        static const std::size_t middleMaxDepth = 5;
        static const std::size_t minBucketCnt = 100;

        Array array{};
        std::size_t size_ = 0;
    };


    template <typename T, typename Hash>
    void swap (HashSet<T, Hash>& lhs, HashSet<T, Hash>& rhs) noexcept {
        lhs.swap (rhs);
    }
}

#endif