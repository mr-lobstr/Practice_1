#ifndef DEFAULT_HASH_H_GUARD
#define DEFAULT_HASH_H_GUARD

#include <string>

namespace data_struct
{
    template <typename T>
    struct Hasher;


    template<>
    struct Hasher<std::string> {
        size_t operator() (std::string const& s) const {
            size_t hash = 2736712;

            for (auto c : s) {
                hash += 37 * c;
            }   
            return hash;
        }
    };

    
    template<>
    struct Hasher<int> {
        size_t operator() (int i) const { 
            return i ^ 701247127;
        }
    };
}

#endif