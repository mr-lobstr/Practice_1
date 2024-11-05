#ifndef MY_HASH_CONTAINER_H_GUARD
#define MY_HASH_CONTAINER_H_GUARD

#include <iostream>
#include "dynamic_array.h"
#include "flist.h"


namespace data_struct
{
    template <typename T, typename Hash, typename Derivied>
    class HashContainerBase {
        using Self = HashContainerBase;

        class IterImpl;
        friend IterImpl;
    
    protected:
        using Bucket = FList<T>;
        using Buckets  = DynamicArray<Bucket>;

        using BucketsIter  = typename Buckets::iterator;
        using ElemsIter = typename Bucket::iterator;

    protected:
        using iterator       = iter::ForwardIterator<T, IterImpl, iter::Mutable_tag>;
        using const_iterator = iter::ForwardIterator<T, IterImpl, iter::Const_tag>;

    protected:
        HashContainerBase() noexcept = default;
        ~HashContainerBase() noexcept = default;

        HashContainerBase (Self&& rhs) noexcept
            : hash (std::move (rhs.hash))
            , buckets (std::move (rhs.buckets))
            , size_ (std::exchange (rhs.size_, 0))
        {}

        HashContainerBase (Self const& rhs) noexcept
            : hash (rhs.hash)
            , buckets (rhs.buckets)
            , size_ (rhs.size_)
        {}

        Self& operator= (Self&& rhs)
        {
            if (this == &rhs) {
                auto tmp {std::move (rhs)};
                swap (tmp);
            }
            return *this;
        }

        Self& operator= (Self const& rhs)
        {
            if (this == &rhs) {
                auto tmp {rhs};
                swap (tmp);
            }
            return *this;
        }
       
        void swap (Self& rhs) noexcept {
            using std::swap;

            swap (hash, rhs.hash);
            swap (buckets, rhs.buckets);
            swap (size_, rhs.size_);
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        std::size_t size() const noexcept {
            return size_;
        }

        auto begin() noexcept {
            return make_begin_iter();
        }

        auto cbegin() const noexcept {
            return make_begin_iter();
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

        bool rehash_before_add() {
            if (buckets.empty()) {
                buckets.resize (minBucketsCnt);
                return true;
            }

            if (size() > buckets.size() * maxCntElemsInBucket) {
                rehash();
                return true;
            }

            return false;
        }
    
    private:
        auto make_begin_iter() const noexcept {
            if (empty())
                return iterator{};

            auto& refBucket= const_cast<Buckets&> (buckets);
            iterator beg (
                refBucket.begin()
              , refBucket.end()
              , refBucket.begin()->prev_begin()
            );

            beg.validate();
            return beg;
        }
        
        auto find_element (T const& elem) {
            return static_cast<Derivied&> (*this).find_element (elem);
        }

        void rehash() {
            Self newSelf;
            newSelf.size_ = size_;
            newSelf.buckets.resize (buckets.size() * 1.5);            
            
            for (auto beg = begin(); beg != end(); beg.validate()) {
                auto it = newSelf.find_element (*beg);

                auto oldPrev = beg.prevElemIt;
                auto& bucket = *it.bucketIt;

                bucket.push_front_node (oldPrev);
            }
            swap (newSelf);
        }

    protected:
        static const std::size_t maxCntElemsInBucket = 5; // среднее значение
        static const std::size_t minBucketsCnt = 5;

        Hash hash{};
        Buckets buckets{};
        std::size_t size_ = 0;
    };


    template <typename T, typename Hash, typename Derivied>
    class HashContainerBase<T, Hash, Derivied>::IterImpl {
        friend Derivied;
        friend HashContainerBase;
    
    public:
        IterImpl() noexcept = default;

        IterImpl (BucketsIter beg, BucketsIter end, ElemsIter elIt) noexcept
            : bucketIt (beg)
            , endIt (end)
            , prevElemIt (elIt)
        {}

    protected:
        bool equal (IterImpl const& rhs) const noexcept {
            if (is_end() or rhs.is_end())
                return is_end() and rhs.is_end();
                
            return prevElemIt == rhs.prevElemIt;
        }

        void validate() noexcept {
            while (is_bucket_end()) {
                ++bucketIt;

                if (bucketIt != endIt) {
                    prevElemIt = bucketIt->prev_begin();
                }
            }
        }

        void next() noexcept {               
            ++prevElemIt;
            validate();
        }

        auto& get_value() const noexcept {
            return *elem_it();
        }
    
    private:
        auto elem_it() const noexcept {
            return next_iter (prevElemIt);
        }

        bool is_bucket_end() const noexcept {
            return bucketIt != endIt
                and elem_it() == bucketIt->end();
        }

        bool is_end() const noexcept {
            return bucketIt == endIt
                or is_bucket_end();
        }

    protected:
        BucketsIter bucketIt{};
        BucketsIter endIt{};
        ElemsIter prevElemIt{};
    };
}

#endif