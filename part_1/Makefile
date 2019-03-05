# Names: Jarrett Knauer, Rafaella Grana
# Student ID: 
# CMPUT 275, WINTER 2019
#
# Assignment 2, Part 1: Driving Route Finder

CC = g++
LFLAGS = 
CFLAGS = -c -Wall -O2 -std=c++11
OBJS = server.o
PROGS = server

# executable targets
server: server.o
	$(CC) server.o -o server $(LFLAGS)

# object targets
server.o: server.cpp server.h
	$(CC) server.cpp -o server.o $(CFLAGS)

# clean
clean:
	@rm $(PROGS) $(OBJS)