CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11

#CSOURCES = \

CXXSOURCES = tree_generator.cpp

OBJECT = tree_generator.o


BINARY = tree_gen \

program: 
#	$(CC) $(CFLAGS) $(CSOURCES)
	$(CXX) $(CXXFLAGS) $(CXXSOURCES)
	$(CXX) $(OBJECT) -o $(BINARY)

all: 
	program


.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o