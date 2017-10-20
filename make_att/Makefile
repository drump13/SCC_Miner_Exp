CXX = g++
CC = gcc
CFLAGS = -c 
CXXFLAGS = -c  -std=c++11

#CSOURCES = \

CXXSOURCES = make_att.cpp \
../trees/tree.cpp \
../trees/file_io.cpp

OBJECT = make_att.o \
tree.o \
file_io.o

BINARY = make_att \

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