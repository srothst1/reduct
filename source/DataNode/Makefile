CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread
# The object files for the program.

OFILES = node.o dataNode.o

HFILES = node.h dataNode.h


all: main

main: $(HFILES) $(OFILES) main.o
			$(CXX) $(CXXFLAGS) -o $@ main.o $(OFILES)

%.o: %.cpp $(HFILES)
			$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f *.o
		rm -f main
