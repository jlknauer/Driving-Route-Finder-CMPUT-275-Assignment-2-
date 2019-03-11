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
    void popMin1();

    // return the number of items held in the heap
    int size() const;

private:
    // the array holding the heap
    std::vector< std::pair<T,K> > heap;

};

// Returns min element in heap by accessing the first
// element in the vector
template <class T, class K>
std::pair<T, K> BinaryHeap<T,K>::min() const{
    return heap.front();
}

// inserts and recomputes the tree so that it is correct
// Args: item: first element of the pair to insert
//       key: second element of the pair to insert
template <class T, class K>
void BinaryHeap<T,K>::insert(const T& item, const K& key){
    // put it at the end
    heap.push_back(make_pair(item,key));
    // position of vertex
    int v = heap.size() - 1;
    int parent = (v-1)/2;
    // fix by swapping content of vertices;
    // repeat process until getting to the root or until achieving order
    while(v > 0 && heap[v].second < heap[parent].second){
        // swap with parent
        std::pair<T,K> aux = heap[v];
        heap[v] = heap[parent];
        heap[parent] = aux;
        // move up the tree
        v = parent;
        parent = (v-1)/2;
    }
}

// removes the minimum key from the heap and restructures the vertices to maintain
// min heap property
template <class T, class K>
void BinaryHeap<T,K>::popMin() {
    heap.front() = heap.back();
    heap.pop_back();
    if (heap.size() == 0) return;

    // root position
    int v = 0;
    int left = 2*v +1;
    int right = 2*v + 2;

    while ((left < heap.size() && heap[left].second < heap[v].second) ||
            (right < heap.size() && heap[right].second < heap[v].second)) {
        // temp tree node for swapping
        pair<T,K> temp = heap[v];
        // both left and right children exist
        if (left < heap.size() && right < heap.size()) {
            // left child is smaller of the two
            if (heap[left].second < heap[right].second) {
                heap[v] = heap[left];
                heap[left] = temp;
                v = left;
            }
            // right child is smaller of the two
            else {
                heap[v] = heap[right];
                heap[right] = temp;
                v = right;
            }
        }
        // only left child exists
        else if (left < heap.size() && right >= heap.size()) {
            heap[v] = heap[left];
            heap[left] = temp;
            v = left;
        }
        // only right child exists
        else if (left >= heap.size() && right < heap.size()) {
            heap[v] = heap[right];
            heap[right] = temp;
            v = right;
        }
        // recompute left/right
        left = 2*v + 1;
        right = 2*v + 2;
    }
}

// getter for the size of the vector holding the heap
template <class T, class K>
int BinaryHeap<T,K>::size() const{
    return heap.size();
}

#endif
