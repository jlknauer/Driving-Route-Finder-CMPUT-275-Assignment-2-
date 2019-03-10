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

#include <iostream>

// computes the least cost paths starting at a given vertex
// args:
//      graph: instance of WDigraph
//      startVertex: a vertex in the search graph which serves as the root of the search tree
//      tree: a search tree used to construct the least cost path to some vertex
// returns:
//      none
void dijkstra(const WDigraph& graph, int startVertex, std::unordered_map<int, PLI>& tree) {
    std::list<PIPLI> events;
    // at the beginning of the search, a vertex with ID == -1 indicates there is no predecessor
    // the non-existent predecessor is assigned a cost of 0
    events.push_back(PIPLI(startVertex, PLI(0,-1)));

    while (!events.empty()) {
        // find a search that reaches its endpoint the earliest
        auto next_event = events.begin();
        for (auto i = events.begin(); i != events.end(); ++i) {
            if (i->second.first < next_event->second.first) {
                next_event = i;
            }
        }
        int vertex = next_event->first;
        int predecessor = next_event->second.second;
        long long cost = next_event->second.first;

        events.erase(next_event);
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
            events.push_back(PIPLI(neighbour, PLI(neighbour_cost, vertex)));
        }
    }
}