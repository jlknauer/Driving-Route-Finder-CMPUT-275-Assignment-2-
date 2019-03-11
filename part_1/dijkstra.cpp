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
    BinaryHeap<int,PLI> events;
    // at the beginning of the search, a vertex with ID == -1 indicates there is no predecessor
    // the non-existent predecessor is assigned a cost of 0
    events.insert(startVertex, PLI(0,-1));

    while (events.size()>0) {
        // minimum
        int vertex = events.min().first;
        int predecessor = events.min().second.second;
        long long cost = events.min().second.first;
        events.popMin();
        // if the predecssor is already in the search tree, it has already been reached by an
        // earlier search; do nothing
        if (tree.find(vertex) != tree.end()) {
            continue;
        }
        // else declare that the vertex has been reached for the cost associated with the
        // predecssor
        tree[vertex] = PLI(cost, predecessor);
        // begin searching all edges leaving the vertex

        for (auto i = graph.neighbours(vertex); i != graph.endIterator(vertex); ++i) {
            int neighbour = *i;
            // the search begins at the vertex with the current cost
            // it will reach the neighbour totalling the current cost plus the edge distance
            int neighbour_cost = cost + graph.getCost(vertex, neighbour);
            events.insert(neighbour, PLI(neighbour_cost, vertex));
        }
    }
}