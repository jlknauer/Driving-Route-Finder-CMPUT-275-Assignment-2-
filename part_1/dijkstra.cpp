// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#include <list>
#include <unordered_map>
#include <utility>
#include "dijkstra.h"
#include "digraph.h"
#include "wdigraph.h"
#include "heap.h"

#include <iostream>

// computes the least cost paths starting at a given vertex
// args:
//      graph: instance of WDigraph
//      startVertex: a vertex in the search graph which serves as the root of the search tree
//      tree: a search tree used to construct the least cost path to some vertex
// returns:
//      none
void dijkstra(const WDigraph& graph, int startVertex, std::unordered_map<int, PLI>& tree) {
    BinaryHeap<std::pair<int,int>, long long> events;
    events.insert(make_pair(startVertex,startVertex), 0);

    while (events.size() > 0) {
        int vertex = events.min().first.second;
        int predecessor = events.min().first.first;
        long long cost = events.min().second;
        // std::cout << "Before: ";
        // for (int i = 0; i < events.size(); ++i) {
        //     std::cout << events.heap[i].second << ' ';
        // }
        events.popMin1();
        // std::cout << "After: ";
        // for (int i = 0; i < events.size(); ++i) {
        //     std::cout << events.heap[i].second << ' ';
        // }
        // std::cout << '\n';

        if (tree.find(vertex) != tree.end()) {
            continue;
        }

        tree[vertex] = PLI(cost, predecessor);

        for (auto i = graph.neighbours(vertex); i != graph.endIterator(vertex); ++i) {
            int neighbour_cost = cost + graph.getCost(vertex, *i);
            events.insert(make_pair(vertex, *i), neighbour_cost);
        }
    }
}