// Names: Jarrett Knauer, Rafaella Grana
// Student ID: 1405557
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 2: Driving Route Finder

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include "dijkstra.h"
#include "serialport.h"
#include "server.h"

// server-client state
bool listening_state = true;

int main() {
    // readGraph and dijkstra are called here
    RouteServer server;
    WDigraph graph;
    coordinates points;
    server.readGraph("edmonton-roads-2.0.1.txt", graph, points);

    // request logic
    SerialPort serial("/dev/ttyACM0");
    int start_vertex_id, end_vertex_id;
    while (true) {
        // listening for client request
        if (listening_state) {
            server.getClientRequest(serial, points, start_vertex_id, end_vertex_id);
        }
        // client request received
        else {
            // generate search tree
            std::unordered_map<int, PLI> search_tree;
            dijkstra(graph, start_vertex_id, search_tree);

            // find path given the search tree
            std::list<int> path;
            server.findPath(path, search_tree, start_vertex_id, end_vertex_id);

            // return waypoints to client if path exists
            // if path exceeds 500 waypoints: return no path
            if (!path.empty() || path.size() <= 500) {
                server.getClientWaypoints(serial, points, path);
            }
            else {
                serial.writeline(response_no_path);
            }
            listening_state = true;
        }
    }
    return 0;
}

void RouteServer::getClientRequest(SerialPort& serial, coordinates& points, int& start_vertex_id, int& end_vertex_id) {
    // listen for route request; no timeout
    std::string request = serial.readline(-1);
    // check for request compliance
    if (request[0] == request_flag) {
        listening_state = false;
        // parse client request
        std::vector<int> space_positions = findCharPositions(' ', request);
        long long start_lat, start_lon, end_lat, end_lon;
        start_lat = std::stoll(request.substr(space_positions[0]+1, space_positions[1]-space_positions[0]));
        start_lon = std::stoll(request.substr(space_positions[1]+1, space_positions[2]-space_positions[1]));
        end_lat = std::stoll(request.substr(space_positions[2]+1, space_positions[3]-space_positions[2]));
        end_lon = std::stoll(request.substr(space_positions[3]+1, request.size()));

        // find the closest vertices to the requested coordinates
        Point start_point = Point(start_lat, start_lon);
        Point end_point = Point(end_lat, end_lon);
        start_vertex_id = findClosest(points, start_point);
        end_vertex_id = findClosest(points, end_point);
    }
}

void RouteServer::getClientWaypoints(SerialPort& serial, coordinates& points, const std::list<int> path) {
    // send number of waypoints in the path
    std::string num_waypoints = "N " + std::to_string(path.size()) + '\n';
    serial.writeline(num_waypoints);

    for (auto i : path) {
        std::string client_response = serial.readline(timeout);
        // client acknowledgement received
        if (client_response == client_ack) {
            std::string waypoint = "W " + std::to_string(points[i].lat) + ' ' + std::to_string(points[i].lon) + '\n';
            serial.writeline(waypoint);
        }
    }
    // path is empty
    // wait for final acknowledgement from client
    std::string final_ack = serial.readline(timeout);
    if (final_ack == client_ack) {
        serial.writeline(end_request);
    }
    // regardless of timeout - return to listening state
    listening_state = true;
    return;

}

// returns a waypoint from the path when client request is received
// args:
//      path: ordered vertices from start vertex to end vertex
//      points: container for Point data structures containing vertex lat/lon
// returns:
//      none
void RouteServer::waypoints(std::list<int>& path, coordinates& points) {
    for (auto i = path.begin(); i != path.end(); ++i) {
        char input_flag;
        std::cin >> input_flag;
        // check for correct client request
        // output format: W lat lon
        if (input_flag == ack_flag) {
            std::cout << waypoint_flag << ' '
                        << points[*i].lat << ' '
                        << points[*i].lon << std::endl;
        }
    }
    // terminate path upon recieving a client request after path is fully iterated through
    char final_flag;
    std::cin >> final_flag;
    if (final_flag == ack_flag) {
        std::cout << end_flag << '\n';
    }
    return;
}

// constructs a path from the start vertex to the end vertex based on the search tree for the 
// start vertex
// args:
//      path: container for ordered vertices from start vertex to end vertex
//      search_tree: search tree constructed with dijkstras in respect to start vertex
//      start_vertex_id: start vertex
//      end_vertex_id: end vertex
// returns:
//      none
void RouteServer::findPath(std::list<int>& path, std::unordered_map<int, PLI> search_tree, 
                            int start_vertex_id, int end_vertex_id) {
    // check if a path from the start to end vertex exists
    if (search_tree.find(end_vertex_id) == search_tree.end()) {
        // respond with no path found and terminate process
        return;
    }
    else {
        // construct path from end vertex to the start vertex
        int stepping = end_vertex_id;
        while (stepping != start_vertex_id) {
            path.push_front(stepping);
            stepping = search_tree[stepping].second;
        }
        path.push_front(start_vertex_id);
    }
    return;
}

// reads in request from the client and locates the closest start and end vertex respectively
// args:
//      points: container for Point data structures containing vertex lat/lon
//      start_vertex_id: start vertex
//      end_vertex_id: end vertex
// returns:
//      none
void RouteServer::getRequest(coordinates& points, int& start_vertex_id, int& end_vertex_id) {
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

// locates the closest vertex for given map coordinates
// args:
//      points: container for Point data structures containing vertex lat/lon
//      location: given map coordinates
// returns:
//      none
int RouteServer::findClosest(coordinates& points, Point& location) {
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

// helper function for parsing based on the position of separators for CSV file
// args:
//      sub: sepearator character
//      s: string to search
// returns:
//      positions: container with the ordered positions of the separators in the given string
std::vector<int> RouteServer::findCharPositions(const char& sub, const string& s) {
    std::vector<int> positions;
    for (std::size_t i = 0; i < s.size()-1; ++i) {
        if (s[i] == sub) positions.push_back(i);
    }
    return positions;
}

// computes manhatten distance between two sets of coordinates
// args:
//      pt1: first point
//      pt2: second point
// returns:
//      (long long): manhatten distance
long long RouteServer::manhatten(const Point& pt1, const Point& pt2) {
    return std::abs(pt1.lat - pt2.lat) + std::abs(pt1.lon - pt2.lon);
}

// constructs graph based on vertex and edge data read from CSV file
// args:
//      filename: name of the CSV file
//      graph: instantiated WDigraph class
//      points: container for the coordinate pairs of the verticies
// returns:
//      none
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
            long double lat_raw = stold(line.substr(sep_positions[1]+1, 
                                            sep_positions[2]-sep_positions[1]));
            long double lon_raw = stold(line.substr(sep_positions[2]+1, line.size()));
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