# CMPUT 275, WINTER 2019
#
# Assignment 2, Part 2: Driving Route Finder

CC = g++
LFLAGS = 
CFLAGS = -c -Wall -O2 -std=c++11
OBJS = server.o digraph.o dijkstra.o serialport.o
PROGS = server

# executable targets
server: $(OBJS)
	$(CC) $(OBJS) -o server $(LFLAGS)

# object targets
server.o: server.cpp server.h digraph.h wdigraph.h serialport.h
	$(CC) server.cpp -o server.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h wdigraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

dijkstra.o: dijkstra.cpp digraph.h wdigraph.h heap.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

serialport.o: serialport.cpp serialport.h
	$(CC) serialport.cpp -o serialport.o $(CFLAGS)

# clean
clean:
	@rm $(PROGS) $(OBJS)
