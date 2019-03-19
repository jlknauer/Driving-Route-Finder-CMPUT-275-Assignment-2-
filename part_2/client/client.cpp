// Names: Jarrett Knauer, Rafaella Grana
// Student ID: 1405557
// CMPUT 275, WINTER 2019
//
// Assignment 2, Part 2: Driving Route Finder

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <SD.h>
#include "communication.h"
#include "consts_and_types.h"
#include "map_drawing.h"

// very simple finite state machine:
  // which endpoint are we waiting for?
// made this global so any func can access
enum {WAIT_FOR_START, WAIT_FOR_STOP} curr_mode = WAIT_FOR_START;
// the variables to be shared across the project, they are declared here!
shared_vars shared;

Adafruit_ILI9341 tft = Adafruit_ILI9341(clientpins::tft_cs, clientpins::tft_dc);

void setup() {
  // initialize Arduino
  init();

  // initialize zoom pins
  pinMode(clientpins::zoom_in_pin, INPUT_PULLUP);
  pinMode(clientpins::zoom_out_pin, INPUT_PULLUP);

  // initialize joystick pins and calibrate centre reading
  pinMode(clientpins::joy_button_pin, INPUT_PULLUP);
  // x and y are reverse because of how our joystick is oriented
  shared.joy_centre = xy_pos(analogRead(clientpins::joy_y_pin), analogRead(clientpins::joy_x_pin));

  // initialize serial port
  Serial.begin(9600);
  Serial.flush(); // get rid of any leftover bits

  // initially no path is stored
  shared.num_waypoints = 0;

  // initialize display
  shared.tft = &tft;
  shared.tft->begin();
  shared.tft->setRotation(3);
  shared.tft->fillScreen(ILI9341_BLUE); // so we know the map redraws properly

  // initialize SD card
  if (!SD.begin(clientpins::sd_cs)) {
      Serial.println("Initialization has failed. Things to check:");
      Serial.println("* Is a card inserted properly?");
      Serial.println("* Is your wiring correct?");
      Serial.println("* Is the chipSelect pin the one for your shield or module?");

      while (1) {} // nothing to do here, fix the card issue and retry
  }

  // initialize the shared variables, from map_drawing.h
  // doesn't actually draw anything, just initializes values
  initialize_display_values();

  // initial draw of the map, from map_drawing.h
  draw_map();
  draw_cursor();

  // initial status message
  status_message("FROM?");
}

void process_input() {
  // read the zoom in and out buttons
  shared.zoom_in_pushed = (digitalRead(clientpins::zoom_in_pin) == LOW);
  shared.zoom_out_pushed = (digitalRead(clientpins::zoom_out_pin) == LOW);

  // read the joystick button
  shared.joy_button_pushed = (digitalRead(clientpins::joy_button_pin) == LOW);

  // joystick speed, higher is faster
  const int16_t step = 64;

  // get the joystick movement, dividing by step discretizes it
  // currently a far joystick push will move the cursor about 5 pixels
  xy_pos delta(
    // the funny x/y swap is because of our joystick orientation
    (analogRead(clientpins::joy_y_pin)-shared.joy_centre.x)/step,
    (analogRead(clientpins::joy_x_pin)-shared.joy_centre.y)/step
  );
  delta.x = -delta.x; // horizontal axis is reversed in our orientation

  // check if there was enough movement to move the cursor
  if (delta.x != 0 || delta.y != 0) {
    // if we are here, there was noticeable movement

    // the next three functions are in map_drawing.h
    erase_cursor();       // erase the current cursor
    move_cursor(delta);   // move the cursor, and the map view if the edge was nudged
    if (shared.redraw_map == 0) {
      // it looks funny if we redraw the cursor before the map scrolls
      draw_cursor();      // draw the new cursor position
    }
  }
}

// draws route
void draw_route(){
  status_message("lets draw this bitch");
  delay(1000);
  if(shared.num_waypoints>0){
    // go thru all but the last index of waypoints
    for(int i=0;i<shared.num_waypoints-1;i++){
      int32_t x0,y0,x1,y1;
      // calculate equivalence for x and y
      x0 = longitude_to_x(shared.map_number,shared.waypoints[i].lon);
      x1 = longitude_to_x(shared.map_number,shared.waypoints[i+1].lon);
      y0 = latitude_to_y(shared.map_number,shared.waypoints[i].lat);
      y1 = latitude_to_y(shared.map_number,shared.waypoints[i+1].lat);
      x0 = x0-shared.map_coords.x;
      x1 = x1-shared.map_coords.x;
      y0 = y0-shared.map_coords.y;
      y1 = y1-shared.map_coords.y;
      bool outside0 = (x0 < 0 or x0 > 320) and (y0 < 0 or y0 > 240);
      bool outside1 = (x1 < 0 or x1 > 320) and (y1 < 0 or y1 > 240);
      if(not outside0 and not outside1){
        shared.tft->drawLine(x0,y0,x1,y1,ILI9341_BLUE);
      }
      
    }
  }
  else{
    // do not draw the waypoints if there arent any
    status_message("NO PATH");
    delay(2000);
  }
  if (curr_mode == WAIT_FOR_STOP){
    status_message("TO?");
  }
  else{
    status_message("FROM?");
  }
}

int main() {
  setup();

  // very simple finite state machine:
  // which endpoint are we waiting for?
  // the two points that are clicked
  lon_lat_32 start, end;

  while (true) {
    // clear entries for new state
    shared.zoom_in_pushed = 0;
    shared.zoom_out_pushed = 0;
    shared.joy_button_pushed = 0;
    shared.redraw_map = 0;

    // reads the three buttons and joystick movement
    // updates the cursor view, map display, and sets the
    // shared.redraw_map flag to 1 if we have to redraw the whole map
    // NOTE: this only updates the internal values representing
    // the cursor and map view, the redrawing occurs at the end of this loop
    process_input();

    // if a zoom button was pushed, update the map and cursor view values
    // for that button push (still need to redraw at the end of this loop)
    // function zoom_map() is from map_drawing.h
    if (shared.zoom_in_pushed) {
      zoom_map(1);
      shared.redraw_map = 1;
    }
    else if (shared.zoom_out_pushed) {
      zoom_map(-1);
      shared.redraw_map = 1;
    }

    // if the joystick button was clicked
    if (shared.joy_button_pushed) {

      if (curr_mode == WAIT_FOR_START) {
        // if we were waiting for the start point, record it
        // and indicate we are waiting for the end point
        start = get_cursor_lonlat();
        curr_mode = WAIT_FOR_STOP;
        status_message("TO?");

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) { delay(20); }
      }
      else {
        // if we were waiting for the end point, record it
        // and then communicate with the server to get the path
        end = get_cursor_lonlat();

        // TODO: communicate with the server to get the waypoints
        // server-client protocol:
        // 1) client sends routing request (server response timeout: 10s)
        // 2) server sends client number of waypoints in routing request (client response timeout: 1s)
        // 3) client acknowledges number of waypoints and asks for waypoints from server (server response timeout: 1s)
        // 4) server sends individual waypoints to client for each acknowledgement (client acknowledgement timeout: 1s)
        // 5) when there are no further waypoints to send, server sends end flag to client
        // 6) client mode switches to waiting for start position
        // 7) server mode switches to listening for client request

        desktopCommunication(shared, start, end);

        // send routing request to the server
        // format: R start_lat start_lon end_lat end_lon<\n> (newline terminated)


        // now we have stored the path length in
        // shared.num_waypoints and the waypoints themselves in
        // the shared.waypoints[] array, switch back to asking for the
        // start point of a new request
        curr_mode = WAIT_FOR_START;

        // wait until the joystick button is no longer pushed
        while (digitalRead(clientpins::joy_button_pin) == LOW) { delay(20); }
        shared.redraw_map = true;
      }
    }

    if (shared.redraw_map) {
      // redraw the status message
      if (curr_mode == WAIT_FOR_START) {
        status_message("FROM?");
      }
      else {
        status_message("TO?");
      }

      // redraw the map and cursor
      draw_map();
      draw_cursor();

      // TODO: draw the route if there is one
      draw_route();
      shared.redraw_map = false;
    }
  }

  Serial.flush();
  return 0;
}
