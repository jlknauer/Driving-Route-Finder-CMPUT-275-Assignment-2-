// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <unordered_map>
#include <utility>
#include "digraph.h"
#include "wdigraph.h"

// alias for a vertex and its associative cost to move to
using PLI = std::pair<long long, int>;
// alias for a vertex and its predecessor pair in the search
using PIPLI = std::pair<int, PLI>;

void dijkstra(const WDigraph& graph, int startVertex, std::unordered_map<int, PLI>& tree);

#endif