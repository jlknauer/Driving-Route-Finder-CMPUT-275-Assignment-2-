// Names: Jarrett Knauer, Rafaella Grana
// Student ID: 1405557
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 2: Driving Route Finder

#ifndef SERVER_H
#define SERVER_h

#include <string>
#include <unordered_map>
#include <vector>
#include "digraph.h"
#include "serialport.h"
#include "wdigraph.h"

const char vertex_flag = 'V';
const char edge_flag = 'E';
const char request_flag = 'R';
const char ack_flag = 'A';
const char waypoint_flag = 'W';
const char path_flag = 'N';
const char end_flag = 'E';
const std::string response_no_path = "N 0\n";
const char space = ' ';

const int timeout = 1000;
const std::string client_ack = "A\n";
const std::string end_request = "E\n";

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

    // gets routing request from stdin
    void getRequest(coordinates& points, int& start_vertex_id, int& end_vertex_id);

    // finds the closest map vertices based on manhatten distance
    int findClosest(coordinates& points, Point& location);

    // constructs path from start to end vertex
    void findPath(std::list<int>& path, std::unordered_map<int, PLI> search_tree, 
                    int start_vertex_id, int end_vertex_id);
    
    // sends waypoints to client
    void waypoints(std::list<int>& path, coordinates& points);

    void getClientRequest(SerialPort& serial, coordinates& points, int& start_vertex_id, int& end_vertex_id);

    void getClientWaypoints(SerialPort& serial, coordinates& points, const std::list<int> path);

private:
};

#endif