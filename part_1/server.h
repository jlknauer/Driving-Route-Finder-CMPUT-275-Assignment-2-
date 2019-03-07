// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#ifndef SERVER_H
#define SERVER_h

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "digraph.h"
#include "wdigraph.h"

const char vertex_flag = 'V';
const char edge_flag = 'E';

struct Point {
    Point() = default;

    Point(long long& input_lat, long long& input_lon) : lat(input_lat), lon(input_lon) {}

    long long lat;
    long long lon;
};

using coordinates = std::unordered_map<int, Point>;

class RouteServer {
public:
    RouteServer() = default;

    // constructs graph of map data from CSV file
    void readGraph(string filename, WDigraph& graph, coordinates& points);

    // identifies substring ranges for parsing
    vector<int> findCharPositions(const char& sub, const string& s);

    // computes manhatten distance between two points
    long long manhatten(const Point& pt1, const Point& pt2);

private:
};

vector<int> RouteServer::findCharPositions(const char& sub, const string& s) {
    vector<int> positions;
    for (std::size_t i = 0; i < s.size()-1; ++i) {
        if (s[i] == sub) positions.push_back(i);
    }
    return positions;
}

long long RouteServer::manhatten(const Point& pt1, const Point& pt2) {
    return std::abs(pt1.lat - pt2.lat) + std::abs(pt1.lon - pt2.lon);
}

void RouteServer::readGraph(string filename, WDigraph& graph, coordinates& points) {
    // read mode
    ifstream file;
    file.open(filename);
    string line;

    // process all lines in the file
    while (getline(file, line)) {
        if (line[0] == vertex_flag) {
            // find separator positions within the line
            vector<int> sep_positions = findCharPositions(',', line);
            // ID substring
            int vertex_id = stoi(line.substr(sep_positions[0]+1,
                                            sep_positions[1]-sep_positions[0]));
            graph.addVertex(vertex_id);
            // lat and lon substring
            float lat_raw = stof(line.substr(sep_positions[1]+1, 
                                            sep_positions[2]-sep_positions[1]));
            float lon_raw = stof(line.substr(sep_positions[2]+1, line.size()));
            long long lat = static_cast<long long> (lat_raw*100000);
            long long lon = static_cast<long long> (lon_raw*100000);
            points[vertex_id] = Point(lat, lon);
        }
        else if (line[0] == edge_flag) {
            // find separator positions within the line
            vector<int> sep_positions = findCharPositions(',', line);
            // vertex substrings
            int vertex1 = stoi(line.substr(sep_positions[0]+1,
                                            sep_positions[1]-sep_positions[0]));
            int vertex2 = stoi(line.substr(sep_positions[1]+1,
                                            sep_positions[2]-sep_positions[1]));
            graph.addEdge(vertex1, vertex2, manhatten(points[vertex1], points[vertex2]));

            return;
        }
    }
}

#endif