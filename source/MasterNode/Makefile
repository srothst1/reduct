CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread


HFILES = secureMap.h secureMap-inl.h inode.h dirInode.h dirInode-inl.h dirInode-inl.h fileInode.h fileInode-inl.h fileDirectory.h fileDirectory-inl.h node.h node-inl.h client.h client-inl.h masterNode.h masterNode-inl.h

all: main

main: $(HFILES) main.o
	$(CXX) $(CXXFLAGS) -o $@ main.o

%.o: %.cpp $(HFILES)
			$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f *.o
		rm -f main
