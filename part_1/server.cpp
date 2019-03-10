// Names: Jarrett Knauer, Rafaella Grana
// Student ID:
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 1: Driving Route Finder

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include "dijkstra.h"
#include "server.h"

int main() {
    // readGraph and dijkstra are called here
    RouteServer server;
    WDigraph graph;
    coordinates points;
    server.readGraph("edmonton-roads-2.0.1.txt", graph, points);

    // request logic
    int start_vertex_id, end_vertex_id;
    server.getRequest(points, start_vertex_id, end_vertex_id);

    // generate search tree
    std::unordered_map<int, PLI> search_tree;
    dijkstra(graph, start_vertex_id, search_tree);

    // read off the path
    std::list<int> path;
    server.findPath(path, search_tree, start_vertex_id, end_vertex_id);
    // print out waypoints if a path exists
    if (!path.empty()) {
        server.waypoints(path, points);
    }
    return 0;
}

void RouteServer::waypoints(std::list<int>& path, coordinates& points) {
    for (auto i = path.begin(); i != path.end(); ++i) {
        char input_flag;
        std::cin >> input_flag;
        if (input_flag == ack_flag) {
            std::cout << waypoint_flag << ' '
                        << points[*i].lat << ' '
                        << points[*i].lon << std::endl;
        }
    }
    char final_flag;
    std::cin >> final_flag;
    std::cout << end_flag << '\n';
    return;
}

void RouteServer::findPath(std::list<int>& path, std::unordered_map<int, PLI> search_tree, 
                            int start_vertex_id, int end_vertex_id) {
    if (search_tree.find(end_vertex_id) == search_tree.end()) {
        std::cout << response_no_path;
        return;
    }
    else {
        int stepping = end_vertex_id;
        while (stepping != start_vertex_id) {
            path.push_front(stepping);
            stepping = search_tree[stepping].second;
        }
        path.push_front(start_vertex_id);
        std::cout << path_flag << ' ' << path.size() << '\n';
    }
    return;
}

void RouteServer::getRequest(coordinates points, int& start_vertex_id, int& end_vertex_id) {
    // valid request is in the format: R start_lat start_lon end_lat end_lon
    char input_flag;
    std::cin >> input_flag;
    if (input_flag == request_flag) {
        long long start_lat, start_lon, end_lat, end_lon;
        std::cin >> start_lat >> start_lon >> end_lat >> end_lon;
        Point start_point = Point(start_lat, start_lon);
        Point end_point = Point(end_lat, end_lon);
        start_vertex_id = findClosest(points, start_point);
        end_vertex_id = findClosest(points, end_point);
    }
}

int RouteServer::findClosest(coordinates points, Point& location) {
    // current closest vertex with its associated manhatten distance
    std::pair<int, long long> closest_vertex;
    bool search_flag = true;
    for (auto vertex : points) {
        // first vertex searched will always be the closest by default
        if (search_flag) {
            closest_vertex.first = vertex.first;
            closest_vertex.second = manhatten(location, vertex.second);
            search_flag = false;
        }
        // if the point is closer than the current closest vertex
        else if (closest_vertex.second > manhatten(location, vertex.second)) {
            closest_vertex.first = vertex.first;
            closest_vertex.second = manhatten(location, vertex.second);
        }
    }
    return closest_vertex.first;
}

std::vector<int> RouteServer::findCharPositions(const char& sub, const string& s) {
    std::vector<int> positions;
    for (std::size_t i = 0; i < s.size()-1; ++i) {
        if (s[i] == sub) positions.push_back(i);
    }
    return positions;
}

long long RouteServer::manhatten(const Point& pt1, const Point& pt2) {
    return std::abs(pt1.lat - pt2.lat) + std::abs(pt1.lon - pt2.lon);
}

void RouteServer::readGraph(std::string filename, WDigraph& graph, coordinates& points) {
    // read mode
    std::ifstream file;
    file.open(filename);
    std::string line;

    // process all lines in the file
    while (getline(file, line)) {
        if (line[0] == vertex_flag) {
            // find separator positions within the line
            std::vector<int> sep_positions = findCharPositions(',', line);
            // ID substring
            int vertex_id = stoi(line.substr(sep_positions[0]+1,
                                            sep_positions[1]-sep_positions[0]));
            graph.addVertex(vertex_id);
            // lat and lon substring
            double lat_raw = stod(line.substr(sep_positions[1]+1, 
                                            sep_positions[2]-sep_positions[1]));
            double lon_raw = stod(line.substr(sep_positions[2]+1, line.size()));
            long long lat = static_cast<long long> (lat_raw*100000);
            long long lon = static_cast<long long> (lon_raw*100000);
            points[vertex_id] = Point(lat, lon);
        }
        else if (line[0] == edge_flag) {
            // find separator positions within the line
            std::vector<int> sep_positions = findCharPositions(',', line);
            // vertex substrings
            int vertex1 = stoi(line.substr(sep_positions[0]+1,
                                            sep_positions[1]-sep_positions[0]));
            int vertex2 = stoi(line.substr(sep_positions[1]+1,
                                            sep_positions[2]-sep_positions[1]));
            graph.addEdge(vertex1, vertex2, manhatten(points[vertex1], points[vertex2]));     
        }
    }
    file.close();
    return;
}