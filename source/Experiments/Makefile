CXX= g++
CXXFLAGS=-g -std=c++11 -Werror -D_GLIBCXX_DEBUG -pthread
SSL = -lcrypto -lssl

ClientProgOFILES = ../ClientProg/hashObject.o ../ClientProg/fileDescriptor.o ../ClientProg/fileObject.o

ClientProgHFILES = ../ClientProg/fileObject.h ../ClientProg/fileDescriptor.h ../ClientProg/hashObject.h


all: exp1-1 exp1-2 exp1-3 exp1-4 exp2-1 exp2-2 exp2-3 exp2-4 exp3-1 exp3-2 exp3-3 exp3-4

exp1-1: $(ClientProgHFILES)  $(ClientProgOFILES) exp1-1.o
			$(CXX) $(CXXFLAGS) -o $@ exp1-1.o $(ClientProgOFILES) $(SSL)

exp1-2: $(ClientProgHFILES)  $(ClientProgOFILES) exp1-2.o
			$(CXX) $(CXXFLAGS) -o $@ exp1-2.o $(ClientProgOFILES) $(SSL)

exp1-3: $(ClientProgHFILES)  $(ClientProgOFILES) exp1-3.o
			$(CXX) $(CXXFLAGS) -o $@ exp1-3.o $(ClientProgOFILES) $(SSL)

exp1-4: $(ClientProgHFILES)  $(ClientProgOFILES) exp1-4.o
			$(CXX) $(CXXFLAGS) -o $@ exp1-4.o $(ClientProgOFILES) $(SSL)

exp2-1: $(ClientProgHFILES)  $(ClientProgOFILES) exp2-1.o
			$(CXX) $(CXXFLAGS) -o $@ exp2-1.o $(ClientProgOFILES) $(SSL)

exp2-2: $(ClientProgHFILES)  $(ClientProgOFILES) exp2-2.o
			$(CXX) $(CXXFLAGS) -o $@ exp2-2.o $(ClientProgOFILES) $(SSL)

exp2-3: $(ClientProgHFILES)  $(ClientProgOFILES) exp2-3.o
			$(CXX) $(CXXFLAGS) -o $@ exp2-3.o $(ClientProgOFILES) $(SSL)

exp2-4: $(ClientProgHFILES)  $(ClientProgOFILES) exp2-4.o
			$(CXX) $(CXXFLAGS) -o $@ exp2-4.o $(ClientProgOFILES) $(SSL)

exp3-1: $(ClientProgHFILES)  $(ClientProgOFILES) exp3-1.o
			$(CXX) $(CXXFLAGS) -o $@ exp3-1.o $(ClientProgOFILES) $(SSL)

exp3-2: $(ClientProgHFILES)  $(ClientProgOFILES) exp3-2.o
			$(CXX) $(CXXFLAGS) -o $@ exp3-2.o $(ClientProgOFILES) $(SSL)

exp3-3: $(ClientProgHFILES)  $(ClientProgOFILES) exp3-3.o
			$(CXX) $(CXXFLAGS) -o $@ exp3-3.o $(ClientProgOFILES) $(SSL)

exp3-4: $(ClientProgHFILES)  $(ClientProgOFILES) exp3-4.o
			$(CXX) $(CXXFLAGS) -o $@ exp3-4.o $(ClientProgOFILES) $(SSL)

%.o: %.cpp $(ClientProgHFILES)
					$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
			$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean all
clean:
		rm -f *.o
		rm -f exp1-1
		rm -f exp1-2
		rm -f exp1-3
		rm -f exp1-4
		rm -f exp2-1
		rm -f exp2-2
		rm -f exp2-3
		rm -f exp2-4
		rm -f exp3-1
		rm -f exp3-2
		rm -f exp3-3
		rm -f exp3-4
