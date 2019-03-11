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
    int v = heap.size();
    v --;
    int parent= (v-1)/2;
    // fix by swapping content of vertices;
    // repeat process until getting to the root or until achieving order
    while(v>0 && heap[v].first < heap[parent].first){
        // swap with parent
        pair<T,K> aux = heap[v];
        heap[v]=heap[parent];
        heap[parent]= aux;
        // move up the tree
        v = parent;
        parent= (v-1)/2;
    }
}

// remove the top and fix tree
// based on pseudocode on the slides
template <class T, class K>
void BinaryHeap<T,K>::popMin(){
    // assign the root the last vertex
    heap.front()=heap.back();
    // remove the last vertex
    heap.pop_back();
    int s = heap.size();
    // make sure heap is not empty
    if (s>0){
        // current vertex position
        int v = 0;
        int right =2*v + 2;
        int left =2*v + 1;
        // while heap property is violated, children are smaller than parent
        while(heap[right].first < heap[v].first or heap[left].first < heap[v].first){
            // create auxiliary vertex
            pair<T,K> aux = heap[v];
            // if left is the smallest
            if(heap[right].first>heap[left].first){
                //swap with left
                heap[v]= heap[left];
                heap[left]= aux;
                // reassign v
                v = left;
            }
            // if right is the smallest
            else{
                // swap with right
                heap[v]= heap[right];
                heap[right]= aux;
                // reassign v
                v = right;
            }
            // recompute right and left
            right =2*v + 2;
            left =2*v + 1;
            // make sure we are not out of bounds
            if (right > s-1){
                // no children
                if (left >s-1){
                    // no more possible swaps
                    // heap is ordered
                    return;
                }
                // one child
                else{
                    // go over the loop once more
                    right = left;
                }

            }
        }
    }
}

// simply returns the size of the vector
template <class T, class K>
int BinaryHeap<T,K>::size() const{
    return heap.size();
}

#endif
