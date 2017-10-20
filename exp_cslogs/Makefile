CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11


CXXSOURCES = exp_cslogs.cpp \
../trees/tree.cpp \
../trees/file_io.cpp \
../trees/lcm_cpp/lcm.cpp

OBJECT = exp_cslogs.o \
tree.o \
file_io.o \
lcm.o

BINARY = exp \

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