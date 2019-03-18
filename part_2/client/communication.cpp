// Names: Jarrett Knauer, Rafaella Grana
// Student ID: 1405557
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 2: Driving Route Finder

#include <Arduino.h>
#include "consts_and_types.h"
#include "communication.h"

void desktopCommunication(shared_vars& shared, const lon_lat_32& start, const lon_lat_32& end) {
    // begin with routing request
    bool request_ack = false;

    // arduino-desktop communications buffer
    uint16_t buffer_len = 0;
    char* buffer = (char*) malloc(buffer_size);
    buffer[buffer_len] = 0;

    // waypoint parsing
    char num_waypoints[4];
    char parse_lat[20];
    char parse_lon[20];
    int waypoint_index = 0;

    // loop until waypoint request is complete
    while (true) {
        // route requisition
        if (!request_ack) {
            // send routing request
            // format: R start_lat start_lon end_lat end_lon
            routingRequest(start, end);
            if (readResponse(buffer, buffer_len, routing_timeout)) {
                // get number of waypoints in path
                // find the index of the newline to parse the numerical value
                char* newline_index = strchr(buffer, '\n');
                strncpy(num_waypoints, buffer+2, (newline_index-(buffer+2)));
                shared.num_waypoints = atoi(num_waypoints);

                request_ack = true;

                // reset buffer
                buffer_len = 0;
                buffer[buffer_len] = 0;
            }
        }
        // waypoint requisition
        else if (request_ack) {
            sendAck();
            if (readResponse(buffer, buffer_len, timeout)) {
                // end of communication session
                // retrieval of all waypoints is successful
                if (buffer[0] == 'E') {
                    buffer_len = 0;
                    buffer[buffer_len] = 0;
                    break;
                }
                if (buffer[0] == 'W') {
                    // find the index of the space separating the lat/lon to parse
                    char* space_index = strchr(buffer+2, ' ');
                    strncpy(parse_lat, buffer+2, (space_index-(buffer+2)));
                    char* newline_index = strchr(buffer, '\n');
                    strncpy(parse_lon, buffer+3+(space_index-(buffer+2)), (newline_index-space_index));
                    shared.waypoints[waypoint_index] = lon_lat_32(atol(parse_lon), atol(parse_lat));
                    ++waypoint_index;
                }
                // reset buffer
                buffer_len = 0;
                buffer[buffer_len] = 0;
            }
            // timeout
            else {
                request_ack = false;
                buffer_len = 0;
                buffer[buffer_len] = 0;
            }
        }
    }
}

// sends routing request given cursor start and end coordinates
// args:
//      start: starting coordinates
//      end: end coordinates
// returns:
//      none
void routingRequest(const lon_lat_32& start, const lon_lat_32&end) {
    Serial.print(routing_flag);
    Serial.print(space);
    Serial.print(start.lat);
    Serial.print(space);
    Serial.print(start.lon);
    Serial.print(space);
    Serial.print(end.lat);
    Serial.print(space);
    Serial.print(end.lon);
    Serial.print(newline);
}

// sends acknowledgement to the server for waypoint receipt
// args:
//      none
// returns:
//      none
void sendAck() {
    Serial.print('A');
    Serial.print(newline);
}

// reads data sent from the desktop over serial
// args:
//      buffer: character array to store incoming data
//      buffer_len: current index for insertion into the buffer
//      timeout: timeout interval in milliseconds
// returns:
//      bool: false if timeout
bool readResponse(char* buffer, uint16_t& buffer_len, const unsigned long timeout) {
    unsigned long start_time = millis();
    char incoming_char;
    // timeout invariant
    while (millis() < (start_time + timeout)) {
        // message waiting in serial
        if (Serial.available()) {
            incoming_char = Serial.read();
            // end of message
            if (incoming_char == newline) {
                // terminate the array with the newline
                buffer[buffer_len] = incoming_char;
                return true;
            }
            // add character to the buffer
            else if (buffer_len < buffer_size-1) {
                buffer[buffer_len] = incoming_char;
                ++buffer_len;
                // drop excess characters
                buffer[buffer_len] = 0;
            }
        }
    }
    // timeout and reset
    buffer_len = 0;
    buffer[buffer_len] = 0;
    return false;
}