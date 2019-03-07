// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#include <iostream>
#include "server.h"

int main() {
    // readGraph and dijkstra are called here
    RouteServer server("edmonton-roads-2.0.1.txt");
    server.readGraph();
    return 0;
}