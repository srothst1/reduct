CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread
# The object files for the program.

SSL = -lcrypto -lssl

OFILES = hashObject.o fileDescriptor.o fileObject.o

HFILES = hashObject.h fileDescriptor.h fileObject.h

all: hashObject.o fileDescriptor.o fileObject.o

%.o: %.cpp $(HFILES)
			$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f *.o
