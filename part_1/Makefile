# Names: Jarrett Knauer, Rafaella Grana
# Student ID:
# CMPUT 275, WINTER 2019
#
# Assignment 2, Part 1: Driving Route Finder

CC = g++
LFLAGS = 
CFLAGS = -c -Wall -O2 -std=c++11
OBJS = server.o digraph.o dijkstra.o
PROGS = server

# executable targets
server: server.o digraph.o dijkstra.o
	$(CC) $(OBJS) -o server $(LFLAGS)

# object targets
server.o: server.cpp server.h digraph.h wdigraph.h
	$(CC) server.cpp -o server.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h wdigraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

dijkstra.o: dijkstra.cpp digraph.h wdigraph.h heap.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

# clean
clean:
	@rm $(PROGS) $(OBJS)