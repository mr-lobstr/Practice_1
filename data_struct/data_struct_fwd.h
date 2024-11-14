#ifndef DATA_STRUCT_FORWARD_H_GUARD
#define DATA_STRUCT_FORWARD_H_GUARD

namespace data_struct
{
    template <typename T> class DynamicArray;
    template <typename T> class FList;
    template <typename T> class List;
    template <typename T> class Stack;
    template <typename T> class Queue;
    template <typename T, typename Hash> class HashSet;
    template <typename Key, typename Value, typename Hash> class HashTable;
    template <typename T> class BinTree;

    template <typename T1, typename T2> class Pair;
}

class StringView;


namespace ds = data_struct;

#endif