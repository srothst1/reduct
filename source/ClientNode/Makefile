CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread
# The object files for the program.

OFILES = node.o fileDescriptor.o client.o clientNode.o 

HFILES = node.h fileDescriptor.h client.h clientNode.h


all: main

main: $(HFILES) $(OFILES) main.o
			$(CXX) $(CXXFLAGS) -o $@ main.o $(OFILES)

%.o: %.cpp $(HFILES)
			$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f *.o
		rm -f main
