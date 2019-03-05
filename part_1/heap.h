// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#ifndef HEAP_H
#define HEAP_H

#include <utility>
#include <vector>

template <class T, class K>
class BinaryHeap {
public:
    BinaryHeap() = default;

    // returns the minimum element in the heap
    std::pair<T, K> min() const;

    // insert an item with a given key
    // if the item is already in the heap, will still insert a new copy with this key
    void insert(const T& item, const K& key);

    // pop the minimum item from the heap
    void popMin();

    // return the number of items held in the heap
    int size() const;

private:
    // the array holding the heap
    std::vector< std::pair<T,K> > heap;
};

#endif